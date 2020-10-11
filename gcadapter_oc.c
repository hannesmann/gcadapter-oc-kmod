#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#define GCADAPTER_VID 0x057e
#define GCADAPTER_PID 0x0337

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hannes Mann");
MODULE_DESCRIPTION("Filter kernel module to set the polling rate of the Wii U/Mayflash GameCube Adapter to a custom value.");
MODULE_VERSION("1.0");

static struct usb_device* adapter_device = NULL;
static unsigned short rate = 2;

static void patch_endpoints(void) {
	if(adapter_device != NULL && adapter_device->actconfig != NULL) {
		struct usb_interface* interface = adapter_device->actconfig->interface[0];
		
		if(interface != NULL) {
			for(unsigned int altsetting = 0; altsetting < interface->num_altsetting; altsetting++) {
				struct usb_host_interface* altsettingptr = &interface->altsetting[altsetting];
			
				for(__u8 endpoint = 0; endpoint < altsettingptr->desc.bNumEndpoints; endpoint++) {
					if(altsettingptr->endpoint[endpoint].desc.bEndpointAddress == 0x81 || altsettingptr->endpoint[endpoint].desc.bEndpointAddress == 0x02) {
						altsettingptr->endpoint[endpoint].desc.bInterval = rate;
					}
				}
			}
		}
		
		usb_reset_device(adapter_device);
	}
}

static int on_usb_notify(struct notifier_block* self, unsigned long action, void* dev) {
	struct usb_device* device = dev;
	
	switch(action) {
		case USB_DEVICE_ADD:
			if(device->descriptor.idVendor == GCADAPTER_VID && device->descriptor.idProduct == GCADAPTER_PID && adapter_device == NULL) {
				adapter_device = device;
				printk(KERN_INFO "gcadapter_oc: Adapter connected\n");
				
				patch_endpoints();
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
				
		patch_endpoints();
	}
	return 0;
}

extern struct bus_type usb_bus_type;
static int __init on_module_init(void) {
	if(rate > 255) {
		printk(KERN_WARNING "gcadapter_oc: Invalid rate parameter specified.\n");
		rate = 255;
	}
	
	if(rate == 0) {
		printk(KERN_WARNING "gcadapter_oc: Invalid rate parameter specified.\n");
		rate = 1;
	}
    	
	usb_for_each_dev(NULL, &bus_device_cb);
	usb_register_notify(&usb_nb);

	return 0;
}

static void __exit on_module_exit(void) {
	if(adapter_device != NULL) {
		rate = 8;
		patch_endpoints();
	}
	
	usb_unregister_notify(&usb_nb);
}

module_init(on_module_init);
module_exit(on_module_exit);

static int on_rate_changed(const char* value, const struct kernel_param* kp) {
	int res = param_set_ushort(value, kp);

    if(res == 0) {		
    	if(rate > 255) { 
			printk(KERN_WARNING "gcadapter_oc: Invalid rate parameter specified.\n");
			rate = 255; 
		}
    	else if(rate == 0) { 
			printk(KERN_WARNING "gcadapter_oc: Invalid rate parameter specified.\n");
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
MODULE_PARM_DESC(rate, "Polling rate (default: 2)");