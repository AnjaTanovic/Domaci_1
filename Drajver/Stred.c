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
#include <linux/wait.h>
#include <linux/semaphore.h>
#define STRING_SIZE 100
#define PRIJEMNI_BUFF 200

MODULE_LICENSE("Dual BSD/GPL");

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

DECLARE_WAIT_QUEUE_HEAD(readQ);
DECLARE_WAIT_QUEUE_HEAD(writeQ);
struct semaphore sem;

char string[STRING_SIZE+1];
int size = 0; //koliko clanova se trenutno nalazi u nizu
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
	int ret;
	char buff[STRING_SIZE];
	long int len;
	
	if (endRead){
		endRead = 0;
		printk(KERN_INFO "Succesfully read\n");
		return 0;
	}

	if (wait_event_interruptible(readQ,(size > 0)))
		return -ERESTARTSYS;

	len = scnprintf(buff,STRING_SIZE , "\nString u baferu je: %s\n", string);
	ret = copy_to_user(buffer, buff, len);
	if(ret)
		return -EFAULT;
	
	endRead = 1;
	return len;
}

ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	char buff[PRIJEMNI_BUFF];
	int ret;

	ret = copy_from_user(buff, buffer, length);
	if(ret)
		return -EFAULT;
	buff[length-1] = '\0';

	if (strstr(buff, "string=")== buff)
	{

		char *ps = buff + 7;
		//da prebrise sve clanove da ne bi se desilo da ostanu neki stari karakteri
		int i;
		for (i = 0; i < STRING_SIZE; i++)
			string[i] = 0;

		if (strlen(ps) <= 100)
		{
			size = strlen(ps);

			strncpy(string, ps, size);
			printk(KERN_WARNING "Upis stringa %s\n", string);
	
		}
		else
		{
			printk(KERN_WARNING "Ne moze se upisati string koji je veci od 100 karaktera\n");
		}
		

		wake_up_interruptible(&readQ);
		wake_up_interruptible(&writeQ);
	}
	else if (strstr(buff, "append=")== buff)
	{
		char *ps = buff + 7;
		int new_size = strlen(ps);
		
		if(down_interruptible(&sem))
			return -ERESTARTSYS;
		while(size + new_size > 100)
		{
			up(&sem);
			if(wait_event_interruptible(writeQ,(size + new_size <= 100)))
				return -ERESTARTSYS;
			if(down_interruptible(&sem))
				return -ERESTARTSYS;
		}		
	//	if(wait_event_interruptible(writeQ,(size + new_size <= 100)))
	//		return -ERESTARTSYS;

		strcat(string, ps);
		printk(KERN_WARNING "Novi string je sad: %s\n", string );

		size = strlen(string);
		
		up(&sem);
		wake_up_interruptible(&readQ);

	}
	else if(!strcmp(buff, "clear"))
	{
		int i;
		for (i = 0; i < STRING_SIZE; i++)
			string[i] = 0;
	
		size = 0;	
		printk(KERN_WARNING "Uspesno obrisan string\n");

		wake_up_interruptible(&writeQ);
	}
	else if(!strcmp(buff, "shrink"))
	{
		char *ps = string;
		
		while (string[0] == ' ')
		{
			size--;
			ps = string + 1; //pomeri se na sledeci i prekopira u string bez ' ' 
			strncpy(string, ps, strlen(string)-1);
			string[strlen(string)-1] = 0;
		}	

		while (string[strlen(string)-1] == ' ')
		{
			size--;
			string[strlen(string)-1] = 0;
		}

		printk(KERN_WARNING "Novi string je sada: %s\n", string);
		wake_up_interruptible(&writeQ);
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
		        if (brisanje > size) 
				brisanje = size;

			for (i = 0; i < brisanje; i++)
			{
				string[size-1] = 0;
				size--;
			}
			
			printk(KERN_WARNING "Izmenjen string string je: %s\n", string);
	      		
			wake_up_interruptible(&writeQ);
	       }
	       else
	       {
			printk(KERN_WARNING "Pogresno uneta komanda\n");
	       }
	}
	else if (strstr(buff, "remove=")== buff)
	{
		int j = 0;
		char *ps = buff + 7;
		int duzina_br = 0;

		char *found;
		int broj_brisanja = 0;
		
		duzina_br = strlen(ps);

			found = strstr(string, ps);
			if (found)
			{
				while(found)
				{
					strncpy(found, found + duzina_br, strlen(found) - duzina_br); 
					found = strstr(string, ps);
					for (j = 1; j <= duzina_br; j++)
						string[strlen(string)-1] = 0;
					printk(KERN_WARNING "Izbrisan string %s iz pocetnog, sada je: %s\n",ps, string);
					broj_brisanja++;
				}
				printk(KERN_WARNING "Uspesno obrisano. Novi string je: %s\n", string);
		
			size = size - duzina_br * broj_brisanja;

			wake_up_interruptible(&writeQ);
			}
			else
				printk(KERN_WARNING "Nema trazenog stringa u pocetnom\n");

	} 
	else 
	{
		printk(KERN_WARNING "Pogresno uneta komanda\n");
	}
	//OBRISATI
	printk(KERN_WARNING "SIZE = %d", size);

	return length;
}

static int __init stred_init(void)
{
   int ret = 0;

   //Inicijalizacija string
   int i;
   for (i = 0; i < STRING_SIZE; i++)
	   string[i] = 0;

   //Inicijalizacija semafora
   sema_init(&sem,1);

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
