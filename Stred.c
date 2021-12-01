#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#define BUFF_SIZE 100

MODULE_LICENSE("Dual BSD/GPL");

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

char string[BUFF_SIZE];
int endRead = 0;

int stred_open(struct inode *pinode, struct file *pfile);
int stred_close(struct inode *pinode, struct file *pfile);
ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = stred_open,
	.read = stred_read,
	.write = stred_write,
	.release = stred_close,
};

int stred_open(struct inode *pinode, struct file *pfile)
{
		printk(KERN_INFO "Succesfully opened file\n");
		return 0;
}

int stred_close(struct inode *pinode, struct file *pfile)
{
		printk(KERN_INFO "Succesfully closed file\n");
		return 0;
}

ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
		printk(KERN_INFO "Succesfully read from file\n");
		return 0;
}

ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	char buff[BUFF_SIZE + 7];
	int ret;
//	char func[BUFF_SIZE + 7]; //jer je najduza mogucnost string= ili append= i jos 100 karaktera

	ret = copy_from_user(buff, buffer, length);
	if(ret)
		return -EFAULT;
	buff[length-1] = '\0';

//	ret = sscanf(buff, "%s", func);
//	if (ret != 1)
//		printk("Pogresna komanda\n");
		
	if (strstr(buff, "string=")== buff)
	{
		char *ps = buff + 7;
		strncpy(string, ps, strlen(buff)-7);
		printk(KERN_WARNING "Upis stringa %s\n", string);
	}
	else if (strstr(buff, "append=")== buff)
	{	
		char *ps = buff + 7;
		strncat(string, ps, strlen(buff)-7);
		printk(KERN_WARNING "Dodat na kraj string %s\n", ps);
	}
	else if(!strcmp(buff, "clear"))
	{
		int i;
		for (i = 0; i < BUFF_SIZE; i++)
			string[i] = 0;
		
		printk(KERN_WARNING "Uspesno obrisan string\n");
	}
	else if(!strcmp(buff, "shrink"))
	{
		char *ps = string;
		int broj_raz = 0; //broji samo za prvi while, jer je za njih potrebno prebrisati
		while (string[0] == ' ')
		{
			broj_raz ++;
			ps = string + 1; //pomeri se na sledeci i prekopira u string bez ' ' 
			strncpy(string, ps, strlen(string)-1);
			string[strlen(string)-1] = 0;
		}	

		while (string[strlen(string)-1] == ' ')
		{
			string[strlen(string)-1] = 0;
		}

		printk(KERN_WARNING "Novi string je sada: %s\n", string);
	}
	else if (strstr(buff, "truncate=")== buff)
	{
		char *ps = buff + 9;
		int ret1;
		int brisanje;
		ret1 = sscanf(ps,"%d",&brisanje);
	       if (ret1 == 1)	
	       {
		       int i;
			for (i = 0; i < brisanje; i++)
				string[strlen(string)-i] = 0;
			
			printk(KERN_WARNING "Izmenjen string string je: %s\n", string);
	       }
	       else
	       {
			printk(KERN_WARNING "Pogresno uneta komanda\n");
	       }
	}
	else if (strstr(buff, "remove=")== buff)
	{

	}
	else 
	{
		printk(KERN_WARNING "Pogresno uneta komanda\n");
	}

	return length;
}

static int __init stred_init(void)
{
   int ret = 0;

   ret = alloc_chrdev_region(&my_dev_id, 0, 1, "stred");
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "stred_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");
   
   my_device = device_create(my_class, NULL, my_dev_id, NULL, "stred");
   if (my_device == NULL){
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
      printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
   printk(KERN_INFO "cdev added\n");
   printk(KERN_INFO "Driver insmoded\n");

   return 0;

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}


static void __exit stred_exit(void)
{
   cdev_del(my_cdev);
   device_destroy(my_class, my_dev_id);
   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,1);
   printk(KERN_INFO "Driver removed\n");
}

module_init(stred_init);
module_exit(stred_exit);
