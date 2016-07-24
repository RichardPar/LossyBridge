// Bridge Mangle - This code creates a lossy network bridge - it simulates a network that is dropping packets
// NOTE : This works on ALL bridge devices
// This code is copyright - GPL v2 License applies
// tuxbox.guru@gmail.com
// This code is a complete example and its hacky as heck! 
// Its prime example is how to do stuff!

#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <net/dst.h>

//#include <linux/neighbour.h>

#include <linux/netfilter_bridge.h>
#include <linux/netfilter_ipv4.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>


int lossyness;

int loss = 0; // Losses ... 0 to 65535 : uses a random short int to determine if the packet should be dropped
int tot  =0;  // Total packets processes
int drop =0;  // Total packets dropped

module_param(loss, int, 0000);
MODULE_PARM_DESC(bridgeDevice, "Lossyness of the bridge [0-65535]");

static int loss_open(struct inode *inode, struct file *file);
static int loss_show(struct seq_file *m, void *v);
static ssize_t loss_write (struct file *file,const char * buf, size_t size, loff_t * ppos);


static const struct file_operations loss_fops = {
    .owner      = THIS_MODULE,
    .open       = loss_open,
    .write      = loss_write,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};


unsigned int nf_icmp_forward_hook(unsigned int hooknum,
                                  struct sk_buff* skb,
                                  const struct net_device* in,
                                  const struct net_device* out,
                                  int (*okfn)(struct sk_buff*));


static struct nf_hook_ops nf_customtrace_ops[] __read_mostly = { { .hook = (nf_hookfn*)nf_icmp_forward_hook,
                                                                   .owner = THIS_MODULE,
                                                                   .pf = NFPROTO_BRIDGE,
                                                                   .hooknum = NF_BR_FORWARD,
                                                                   .priority = NF_BR_PRI_FILTER_BRIDGED, },};
                                                                 

unsigned int nf_icmp_forward_hook(unsigned int hooknum,
                                  struct sk_buff* skb,
                                  const struct net_device* in,
                                  const struct net_device* out,
                                  int (*okfn)(struct sk_buff*))
{
   unsigned short ran;

    if (skb) {
        tot++;
        get_random_bytes(&ran, 2);
        if (ran <= loss)
          {
            printk("Packet Dropped\r\n");
            drop++;
            return NF_DROP; 
           }


    }
    return NF_ACCEPT;
}


static int loss_show(struct seq_file *m, void *v)
{
    seq_printf(m, "Loss : %d\nIn : %d\nDrop :%d\n", loss,tot,drop);
    return 0;
}

static int loss_open(struct inode *inode, struct file *file)
{
    return single_open(file, loss_show, NULL);
}

static ssize_t loss_write (struct file *file,const char * buf, size_t size, loff_t * ppos)
{
 
  size_t len = 20;
  char mbuf[21];
  unsigned int val=0;
  int err;


  if (len > size)
      len = size;

  if (copy_from_user (mbuf, buf, len))
    return -EFAULT;

  mbuf[len] = '\0';

  err = kstrtouint(mbuf, 0, &val); 
  if (err)
   {
      printk("Error converting %s\r\n",mbuf);
      return -1;
    } else
    {
      loss=val;
      tot=0;
      drop=0;
    }

  return len;
}

int init_module() 
{
    int ret;
    

    proc_create("loss", 0, NULL, &loss_fops);

 
    lossyness = loss;

    ret = nf_register_hooks(nf_customtrace_ops, ARRAY_SIZE(nf_customtrace_ops));
    if (ret < 0) {
        printk("register nf hook fail\n");
        return ret;
    }
    printk(KERN_NOTICE "BridgeMangler\n");
    return 0;
}

void cleanup_module()
{
     remove_proc_entry("loss", NULL);
    nf_unregister_hooks(nf_customtrace_ops, ARRAY_SIZE(nf_customtrace_ops));
}

MODULE_LICENSE("GPL");
