#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#include "gcadapter.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hannes Mann");
MODULE_DESCRIPTION("Filter kernel module to set the polling rate of the Wii U/Mayflash GameCube Adapter to a custom value.");
MODULE_VERSION("1.0");

static struct usb_device* adapter_device;
static unsigned short rate = GCADAPTER_RECOMMENDED_INTERVAL;

static void patch_endpoints(void)
{
	if(adapter_device != NULL && adapter_device->actconfig != NULL)
	{
		struct usb_interface* interface = adapter_device->actconfig->interface[0];
		
		if(interface != NULL)
		{
			for(unsigned int altsetting = 0; altsetting < interface->num_altsetting; altsetting++)
			{
				struct usb_host_interface* altsettingptr = &interface->altsetting[altsetting];
			
				for(__u8 endpoint = 0; endpoint < altsettingptr->desc.bNumEndpoints; endpoint++)
				{
					if(altsettingptr->endpoint[endpoint].desc.bEndpointAddress == 0x81 || altsettingptr->endpoint[endpoint].desc.bEndpointAddress == 0x02)
					{
						altsettingptr->endpoint[endpoint].desc.bInterval = rate;
						
						printk(KERN_INFO "gcadapter_oc_kmod: [altsetting %d] -> [endpoint %d addr 0x%x] rate %dhz applied\n", 
							altsetting, 
							endpoint, 
							altsettingptr->endpoint[endpoint].desc.bEndpointAddress, 
							1000 / rate);
					}
				}
			}
		}
		
		usb_reset_device(adapter_device);
	}
}

static int on_usb_notified(struct notifier_block* self, unsigned long action, void* dev)
{
	struct usb_device* device = dev;
	
	switch(action)
	{
		case USB_DEVICE_ADD:
			if(device->descriptor.idVendor == GCADAPTER_VID && device->descriptor.idProduct == GCADAPTER_PID && adapter_device == NULL)
			{
				adapter_device = device;
				printk(KERN_INFO "gcadapter_oc_kmod: New adapter connected\n");
				
				patch_endpoints();
			}
			break;
			
		case USB_DEVICE_REMOVE:
			if(device->descriptor.idVendor == GCADAPTER_VID && device->descriptor.idProduct == GCADAPTER_PID && adapter_device != NULL)
			{
				adapter_device = NULL;
				printk(KERN_INFO "gcadapter_oc_kmod: Adapter disconnected\n");
			}
			break;
	}
	
	return NOTIFY_OK;
}

// https://stackoverflow.com/questions/5911849/simple-kernel-module-for-usb?rq=1
// Seems to be "bad", but only way to do it w/o unloading usbhid
static struct notifier_block usb_nb = 
{ 
    .notifier_call = on_usb_notified
};

// https://stackoverflow.com/questions/34957016/signal-on-kernel-parameter-change
static int on_rate_changed(const char* value, const struct kernel_param* kp)
{
	int res = param_set_ushort(value, kp);
    if(res == 0)
    {
		printk(KERN_INFO "gcadapter_oc_kmod: Rate module parameter changed to %dhz\n", 1000 / rate);
		
    	if(rate > 255)
    	{
    		printk(KERN_WARNING "gcadapter_oc_kmod [warning]: Rate parameter invalid\n");
    		rate = 255;
    	}
    	
    	if(rate == 0)
    	{
    		printk(KERN_WARNING "gcadapter_oc_kmod [warning]: Rate parameter was invalid, set to 1000hz\n");
    		rate = 1;
    	}
    	
		patch_endpoints();
    }

	return res;
}

static struct kernel_param_ops rate_ops =
{
    .set = &on_rate_changed,
    .get = &param_get_ushort
};

module_param_cb(rate, &rate_ops, &rate, 0644);
MODULE_PARM_DESC(rate, "Polling rate to set for the adapter (default: 2).");

static int __init on_mod_init(void)
{
	if(rate > 255)
	{
		printk(KERN_WARNING "gcadapter_oc_kmod [warning]: Rate parameter was invalid, sanitized to 3.9hz\n");
		rate = 255;
	}
	
	if(rate == 0)
	{
		printk(KERN_WARNING "gcadapter_oc_kmod [warning]: Rate parameter was invalid, sanitized to 1000hz\n");
		rate = 1;
	}
    	
	adapter_device = NULL;
	usb_register_notify(&usb_nb);
	return 0;
}

static void __exit on_mod_exit(void)
{
	if(adapter_device != NULL)
	{
		printk(KERN_INFO "gcadapter_oc_kmod: Restoring default rate for adapter\n");
		rate = 8;
		patch_endpoints();
		
		adapter_device = NULL;
	}
	
	usb_unregister_notify(&usb_nb);
}

module_init(on_mod_init);
module_exit(on_mod_exit);
