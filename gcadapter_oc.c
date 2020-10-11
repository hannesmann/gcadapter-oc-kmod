#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#define GCADAPTER_VID 0x057e
#define GCADAPTER_PID 0x0337

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hannes Mann");
MODULE_DESCRIPTION("Filter kernel module to set the polling rate of the Wii U/Mayflash GameCube Adapter to a custom value.");
MODULE_VERSION("1.1");

static struct usb_device* adapter_device = NULL;

static unsigned short restore_interval = 8;
static unsigned short configured_interval = 2;

/* Patches all applicable endpoints. Returns the bInterval value used before patching. */
static unsigned short patch_endpoints(unsigned short interval) {
	static unsigned short old_interval = 8;

	if(adapter_device != NULL && adapter_device->actconfig != NULL) {
		struct usb_interface* interface = adapter_device->actconfig->interface[0];
		
		if(interface != NULL) {
			for(unsigned int altsetting = 0; altsetting < interface->num_altsetting; altsetting++) {
				struct usb_host_interface* altsettingptr = &interface->altsetting[altsetting];
			
				for(__u8 endpoint = 0; endpoint < altsettingptr->desc.bNumEndpoints; endpoint++) {
					if(altsettingptr->endpoint[endpoint].desc.bEndpointAddress == 0x81 || altsettingptr->endpoint[endpoint].desc.bEndpointAddress == 0x02) {
						old_interval = altsettingptr->endpoint[endpoint].desc.bInterval;
						altsettingptr->endpoint[endpoint].desc.bInterval = interval;
					}
				}
			}
		}
		
		usb_reset_device(adapter_device);
	}

	return old_interval;
}

static int on_usb_notify(struct notifier_block* self, unsigned long action, void* dev) {
	struct usb_device* device = dev;
	
	switch(action) {
		case USB_DEVICE_ADD:
			if(device->descriptor.idVendor == GCADAPTER_VID && device->descriptor.idProduct == GCADAPTER_PID && adapter_device == NULL) {
				adapter_device = device;
				printk(KERN_INFO "gcadapter_oc: Adapter connected\n");
				
				restore_interval = patch_endpoints(configured_interval);
			}
			break;
			
		case USB_DEVICE_REMOVE:
			if(device->descriptor.idVendor == GCADAPTER_VID && device->descriptor.idProduct == GCADAPTER_PID && adapter_device == device) {
				adapter_device = NULL;
				printk(KERN_INFO "gcadapter_oc: Adapter disconnected\n");
			}
			break;
	}
	
	return NOTIFY_OK;
}

static struct notifier_block usb_nb = { .notifier_call = on_usb_notify };

static int bus_device_cb(struct usb_device* device, void* data) {
	if(device->descriptor.idVendor == GCADAPTER_VID && device->descriptor.idProduct == GCADAPTER_PID && adapter_device == NULL) {
		adapter_device = device;
		printk(KERN_INFO "gcadapter_oc: Adapter connected\n");
				
		restore_interval = patch_endpoints(configured_interval);
	}
	return 0;
}

extern struct bus_type usb_bus_type;
static int __init on_module_init(void) {
	if(configured_interval > 255) {
		printk(KERN_WARNING "gcadapter_oc: Invalid interval parameter specified.\n");
		configured_interval = 255;
	}
	
	if(configured_interval == 0) {
		printk(KERN_WARNING "gcadapter_oc: Invalid interval parameter specified.\n");
		configured_interval = 1;
	}
    	
	usb_for_each_dev(NULL, &bus_device_cb);
	usb_register_notify(&usb_nb);

	return 0;
}

static void __exit on_module_exit(void) {
	if(adapter_device != NULL) {
		patch_endpoints(restore_interval);
	}
	
	usb_unregister_notify(&usb_nb);
}

module_init(on_module_init);
module_exit(on_module_exit);

static int on_interval_changed(const char* value, const struct kernel_param* kp) {
	int res = param_set_ushort(value, kp);

    if(res == 0) {		
    	if(configured_interval > 255) { 
			printk(KERN_WARNING "gcadapter_oc: Invalid interval parameter specified.\n");
			configured_interval = 255; 
		}
    	else if(configured_interval == 0) { 
			printk(KERN_WARNING "gcadapter_oc: Invalid interval parameter specified.\n");
			configured_interval = 1; 
		}
    	
		patch_endpoints(configured_interval);
    }

	return res;
}

static struct kernel_param_ops interval_ops =
{
    .set = &on_interval_changed,
    .get = &param_get_ushort
};

module_param_cb(interval, &interval_ops, &configured_interval, 0644);
MODULE_PARM_DESC(interval, "Polling rate (default: 2)");