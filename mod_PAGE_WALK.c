#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/kvm_host.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/kernel.h>


#define NETLINK_USER 31

struct sock *nl_sk = NULL;

char *receive_string (struct sk_buff *skb, struct nlmsghdr **nlh)
{
    *nlh = (struct nlmsghdr *)skb->data;
    return (char *)nlmsg_data(*nlh);
}

void send_string (char *string, struct sk_buff *skb, struct nlmsghdr *nlh)
{
    int msg_size;
    int pid, res;
    struct sk_buff *skb_out;
    
    msg_size = strlen(string)+1;
    pid = nlh->nlmsg_pid; /*pid of sending process */
    skb_out = nlmsg_new(msg_size, 0);

    if (!skb_out)
    {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return;
    }

    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    strncpy(nlmsg_data(nlh), string, msg_size);
    res = nlmsg_unicast(nl_sk, skb_out, pid);

    if (res < 0)
        printk(KERN_INFO "Error while sending bak to user\n");
}

static void hello_nl_recv_msg(struct sk_buff *skb)
{

    struct nlmsghdr *nlh;
    char msg[256];
    char *msg_rec;
    struct task_struct *ts;
    struct mm_struct *mms;
    long unsigned int lin;

    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);
    
    msg_rec = receive_string(skb, &nlh);
    printk(KERN_INFO "%s\n", msg_rec);
	//sprintf(msg, "%d", PAGE_SIZE);
	ts = get_current();
	printk(KERN_INFO "%s\n", ts->comm);
	mms = ts->mm;
	printk(KERN_INFO "lin addr pgd = 0x%16.16lx\n", mms->pgd);
    sscanf(msg_rec, "%lu", &lin);
    printk(KERN_INFO "lin addr sent = 0x%16.16lx\n", lin);
    lin = lin<<16;
    printk(KERN_INFO "lin addr modded = 0x%16.16lx\n", lin);
    lin = lin>>55;
    printk(KERN_INFO "lin addr modded = 0x%16.16lx\n", lin);



    send_string(msg, skb, nlh);
}

static int __init hello_init(void)
{

    struct netlink_kernel_cfg cfg = {
        .groups = 1,
        .input = hello_nl_recv_msg,
    };
	printk("Entering: %s\n", __FUNCTION__);
    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk)
    {

        printk(KERN_ALERT "Error creating socket.\n");
        return -10;

    }

    return 0;
}

static void __exit hello_exit(void)
{

    printk(KERN_INFO "exiting hello module\n");
    netlink_kernel_release(nl_sk);
}

module_init(hello_init); module_exit(hello_exit);

MODULE_LICENSE("GPL");
