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
    unsigned long int lin, pgd_index, pmd_index, pud_index, pte_index, offset, cr3, word_addr;
    unsigned long int *pgd, *pud_phys, *pmd_phys, *pte_phys, *page_phys, *pmd, *pte, *pud, *page, *lin_cast; 
    char *word;



    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);
    
    msg_rec = receive_string(skb, &nlh);
    printk(KERN_INFO "%s\n", msg_rec);
	//sprintf(msg, "%d", PAGE_SIZE);
	ts = get_current();
	printk(KERN_INFO "%s\n", ts->comm);
	mms = ts->mm;
    pgd = mms->pgd;
	printk(KERN_INFO "lin addr pgd = 0x%16.16lx\n", pgd);
    printk(KERN_INFO "lin addr pgd = %lu\n", pgd);
    printk(KERN_INFO "phys addr pgd = %lu\n", pgd-PAGE_OFFSET/8);
    printk(KERN_INFO "phys addr pgd = 0x%16.16lx\n", pgd-PAGE_OFFSET/8);
    __asm__ __volatile__ (
        "mov %%cr3, %%rax\n"
        "mov %%rax, %0\n"
    :  "=m" (cr3)
    :
    :  "%rax"
    );
    printk(KERN_INFO "cr3 = 0x%16.16lx\n", cr3);
    sscanf(msg_rec, "%lu", &lin);
    printk(KERN_INFO "lin addr sent = 0x%16.16lx\n", lin);
    printk(KERN_INFO "PAGE_OFFSET = %lu\n", PAGE_OFFSET);
    pgd_index = lin<<16;
    pgd_index = pgd_index>>55;
    printk(KERN_INFO "pgd index = 0x%16.16lx\n", pgd_index);
    printk(KERN_INFO "pgd index = %lu\n", pgd_index);
    pud_phys = pgd[pgd_index];
    pud_phys = (unsigned long int)pud_phys>>12;
    pud_phys = (unsigned long int)pud_phys<<12;
    pud = pud_phys + PAGE_OFFSET/8;
    pud_index = lin<<25;
    pud_index = pud_index>>55;
    printk(KERN_INFO "                    %lu\n", pgd);

    printk(KERN_INFO "pgd[index] = 0x%16.16lx\n", pud_phys);
    printk(KERN_INFO "pgd[index] =   %lu\n", pud_phys);
    printk(KERN_INFO "pud index = 0x%16.16lx\n", pud_index);
    pmd_phys = pud[pud_index];
    pmd_phys = (unsigned long int)pmd_phys>>12;
    pmd_phys = (unsigned long int)pmd_phys<<12;
    pmd = pmd_phys + PAGE_OFFSET/8;
    pmd_index = lin<<34;
    pmd_index = pmd_index>>55;
    printk(KERN_INFO "pud[index] = 0x%16.16lx\n", pmd_phys);
    printk(KERN_INFO "pud[index] =   %lu\n", pmd_phys);
    printk(KERN_INFO "pmd index = 0x%16.16lx\n", pmd_index);
    pte_phys = pmd[pmd_index];
    pte_phys = (unsigned long int)pte_phys>>12;
    pte_phys = (unsigned long int)pte_phys<<12;
    pte = pte_phys + PAGE_OFFSET/8;
    pte_index = lin<<43;
    pte_index = pte_index>>55;
    printk(KERN_INFO "pmd[index] = 0x%16.16lx\n", pte_phys);
    printk(KERN_INFO "pmd[index] =   %lu\n", pte_phys);
    printk(KERN_INFO "pte index = 0x%16.16lx\n", pte_index);
    page_phys = pte[pte_index];
    page_phys = (unsigned long int)page_phys>>12;
    page_phys = (unsigned long int)page_phys<<12;
    page_phys = (unsigned long int)page_phys<<28;
    page_phys = (unsigned long int)page_phys>>28;
    page = page_phys + PAGE_OFFSET/8;
    offset = lin<<52;
    offset = offset>>52;
    printk(KERN_INFO "pte[index] = 0x%16.16lx\n", page_phys);
    printk(KERN_INFO "pte[index] =   %lu\n", page_phys);
    printk(KERN_INFO "offset = 0x%16.16lx\n", offset);
    word_addr = (unsigned long int)page + offset;
    printk(KERN_INFO "word address = 0x%16.16lx\n", word_addr);
    printk(KERN_INFO "word = %s\n", (char *)word_addr);
    lin_cast = (char *)lin;
    printk(KERN_INFO "word = %s\n", lin_cast);







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
