/*
 * USB Serial Converter Generic Low-Speed
 *
 * Copyright (C) 2021 Alex Mansfield (alimansfield2011@gmail.com)
 */

#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/sysrq.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>
#include <linux/serial.h>


#define DRIVER_AUTHOR "Alex Mansfield, alimansfield2011@gmail.com, alex.mansfields.org"
#define DRIVER_DESC "Low-speed USB Serial driver"
#define DRIVER_VER "1.02"

#define VENDOR_ID	0x16C0
#define PRODUCT_ID	0x05DC


/* table of devices that work with this driver */
static struct usb_device_id id_table [] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{ },
};
MODULE_DEVICE_TABLE (usb, id_table);
/*
static int usbserial_ls_setup_port_int_in(struct usb_serial_port *port, struct usb_endpoint_descriptor *epd)
{
	struct usb_serial_driver *drv;
	struct usb_device *udev;
	int buffer_size;
	int i;
	drv = port->serial->type;
	udev = port->serial->dev;

	port->interrupt_in_urb = usb_alloc_urb(0, GFP_KERNEL);
	if(!port->interrupt_in_urb)
		return -ENOMEM;
	buffer_size = usb_endpoint_maxp(epd);
	port->interrupt_in_endpointAddress = epd->bEndpointAddress;
	port->interrupt_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
	if(!port->interrupt_in_buffer)
		return -ENOMEM;
	usb_fill_int_urb(port->interrupt_in_urb, udev,
						usb_rcvintpipe(udev, epd->bEndpointAddress),
						port->interrupt_in_buffer, buffer_size,
						drv->read_int_callback, port,
						epd->bInterval);
	return 0;
}
static int usbserial_ls_setup_port_int_out(struct usb_serial_port *port, struct usb_endpoint_descriptor *epd)
{
	struct usb_serial_driver *drv;
	struct usb_device *udev;
	int buffer_size;
	// int i;
	drv = port->serial->type;
	udev = port->serial->dev;
	// buffer_size = 
	port->interrupt_out_urb = usb_alloc_urb(0, GFP_KERNEL);
	if(!port->interrupt_out_urb)
		return -ENOMEM;
	port->interrupt_out_size = buffer_size;
	port->interrupt_out_endpointAddress = epd->bEndpointAddress;
	port->interrupt_out_buffer = kmalloc(buffer_size, GFP_KERNEL);
	if (!port->interrupt_out_buffer)
		return -ENOMEM;
	usb_fill_int_urb(port->interrupt_out_urb, udev,
			usb_sndintpipe(udev, epd->bEndpointAddress),
			port->interrupt_out_buffer, buffer_size,
			drv->write_int_callback, port,
			epd->bInterval);

	return 0;
	
}
*/
static int usbserial_ls_probe(struct usb_serial *serial, const struct usb_device_id *id)
{
	struct device *dev;
	dev = &serial->interface->dev;
	dev_dbg(dev, "The \""DRIVER_DESC"\" is only for testing and one-off prototypes.\n");

	return 0;
}
static int usbserial_ls_attach(struct usb_serial *serial)
{
	struct device *dev;
	dev = &serial->interface->dev;
	dev_dbg(dev, "usbserial_ls_attach()\n");
	return 0;
}

static int usbserial_ls_calc_num_ports(struct usb_serial *serial, struct usb_serial_endpoints *epds)
{
	struct device *dev;
	int num_ports;
	dev = &serial->interface->dev;

	num_ports = max(epds->num_interrupt_in, epds->num_interrupt_out);

	if (num_ports == 0) {
		dev_err(dev, "device has no interrupt endpoints\n");
		return -ENODEV;
	}

	return num_ports;
}
/*
static void usbserial_ls_disconnect(struct usb_serial *serial)
{
	struct device *dev;
	dev = &serial->interface->dev;
	dev_dbg(dev, "usbserial_ls_disconnect()\n");

	// return 0;
}

static void usbserial_ls_release(struct usb_serial *serial)
{
	struct device *dev;
	int i;
	dev = &serial->interface->dev;
	dev_dbg(dev, "usbserial_ls_release()\n");

}

static void usbserial_ls_port_release(struct device *dev)
{
	struct usb_serial_port *port;
	int i;
	port = to_usb_serial_port(dev);
	dev_dbg(dev, "usbserial_ls_release()\n");

	usb_free_urb(port->interrupt_in_urb);
	usb_free_urb(port->interrupt_out_urb);
	for (i = 0; i < ARRAY_SIZE(port->read_urbs); ++i) {
		usb_free_urb(port->read_urbs[i]);
		kfree(port->bulk_in_buffers[i]);
	}
	for (i = 0; i < ARRAY_SIZE(port->write_urbs); ++i) {
		usb_free_urb(port->write_urbs[i]);
		kfree(port->bulk_out_buffers[i]);
	}
	kfifo_free(&port->write_fifo);
	kfree(port->interrupt_in_buffer);
	kfree(port->interrupt_out_buffer);
	tty_port_destroy(&port->port);
	kfree(port);
}
*/
static int usbserial_ls_port_probe(struct usb_serial_port *port)
{
	struct device *dev;
	dev = &port->serial->interface->dev;

	if (kfifo_alloc(&port->write_fifo, PAGE_SIZE, GFP_KERNEL))
		return -ENOMEM;
	dev_dbg(dev, "usbserial_ls_port_probe()\n");
	return 0;
}
/*
static int usbserial_ls_port_remove(struct usb_serial_port *port)
{
	struct device *dev;
	dev = &port->serial->interface->dev;
	dev_dbg(dev, "usbserial_ls_port_remove()\n");
	kfifo_free(&port->write_fifo);
	return 0;
}


static int usbserial_ls_suspend(struct usb_serial *serial, pm_message_t message)
{
	struct device *dev;
	dev = &serial->interface->dev;
	dev_dbg(dev, "usbserial_ls_suspend()\n");

	return 0;
}
static int usbserial_ls_resume(struct usb_serial *serial)
{
	struct device *dev;
	dev = &serial->interface->dev;
	dev_dbg(dev, "usbserial_ls_resume()\n");

	return 0;
}
static int usbserial_ls_reset_resume(struct usb_serial *serial)
{
	struct device *dev;
	dev = &serial->interface->dev;
	dev_dbg(dev, "usbserial_ls_reset_resume()\n");

	return 0;
}

*/
	/* serial function calls */
	/* Called by console and by the tty layer */
static int  usbserial_ls_open(struct tty_struct *tty, struct usb_serial_port *port)
{
	struct device *dev;
	dev  = &port->serial->interface->dev;
	dev_dbg(dev, "usbserial_ls_open()\n");
	// return usb_serial_generic_open(tty, port);

	if(port->interrupt_out_urb){
		port->interrupt_out_urb->transfer_buffer_length = 0;
		usb_submit_urb(port->interrupt_out_urb, GFP_KERNEL);
	}
	else
		dev_dbg(dev, "NULL port->interrupt_out_urb");
	if(port->interrupt_in_urb){
		usb_submit_urb(port->interrupt_in_urb, GFP_KERNEL);
	}
	else
		dev_dbg(dev, "NULL port->interrupt_in_urb");
	return 0;
}
static void usbserial_ls_close(struct usb_serial_port *port)
{
	struct device *dev;
	dev = &port->serial->interface->dev;
	dev_dbg(dev, "usbserial_ls_close()\n");

	usb_kill_urb(port->interrupt_out_urb);
	usb_kill_urb(port->interrupt_in_urb);
	
}
static int  usbserial_ls_write(struct tty_struct *tty, struct usb_serial_port *port, const unsigned char *buf, int count)
{
	struct device *dev;
	dev = &port->serial->interface->dev;
	dev_dbg(dev, "usbserial_ls_write(); count=%i\n", count);

	// int result;

	if (!port->interrupt_out_size)
		return -ENODEV;

	if (!count)
		return 0;
	if(!port->write_fifo.kfifo.data)
		printk(KERN_INFO "write_fifo.data=NULL");

	count = kfifo_in_locked(&port->write_fifo, buf, count, &port->lock);
	// result = usb_serial_generic_write_start(port, GFP_ATOMIC);
	// if (result)
		// return result;
	dev_dbg(dev, "usbserial_ls_write(); return=%i\n", count);
	return count;
}


	/* Called only by the tty layer */
static int  usbserial_ls_write_room(struct tty_struct *tty)
{
	struct usb_serial_port *port;
	struct device *dev;
	unsigned long flags;
	int room;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_write_room()");

	if (!port->interrupt_out_size)
		return 0;

	spin_lock_irqsave(&port->lock, flags);
	room = kfifo_avail(&port->write_fifo);
	spin_unlock_irqrestore(&port->lock, flags);

	dev_dbg(&port->dev, "%s - returns %d\n", __func__, room);
	return room;
}
/* static int  usbserial_ls_ioctl(struct tty_struct *tty, unsigned int cmd, unsigned long arg)
{
	struct usb_serial_port *port;
	struct device *dev;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_ioctl()");
	// return usb_serial_generic_ioctl(tty, cmd, arg);
	return 0;
}
static int  usbserial_ls_get_serial(struct tty_struct *tty, struct serial_struct *ss)
{
	struct usb_serial_port *port;
	struct device *dev;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_get_serial()");
	// return usb_serial_generic_get_serial(tty, ss);
	return 0;
}
static int  usbserial_ls_set_serial(struct tty_struct *tty, struct serial_struct *ss)
{
	struct usb_serial_port *port;
	struct device *dev;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_set_serial()");
	// return usb_serial_generic_set_serial(tty, ss);
	return 0;
}
static void usbserial_ls_set_termios(struct tty_struct *tty, struct usb_serial_port *port, struct ktermios *old)
{
	struct device *dev;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_set_termios()");
}
static void usbserial_ls_break_ctl(struct tty_struct *tty, int break_state)
{
	struct usb_serial_port *port;
	struct device *dev;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_break_ctl()");
}
static int  usbserial_ls_chars_in_buffer(struct tty_struct *tty)
{
	struct usb_serial_port *port;
	struct device *dev;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_chars_in_buffer()");
	return usb_serial_generic_chars_in_buffer(tty);
}
static void usbserial_ls_wait_until_sent(struct tty_struct *tty, long timeout)
{
	struct usb_serial_port *port;
	struct device *dev;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_wait_until_sent()");
	return usb_serial_generic_wait_until_sent(tty, timeout);
}*/
static bool usbserial_ls_tx_empty(struct usb_serial_port *port)
{
	struct device *dev;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_tx_empty()");
	if(kfifo_len(&port->write_fifo) || port->interrupt_out_urb->actual_length){
		return false;
	}
	// return usb_serial_generic_tx_empty(port);
	return true;
}
/* static void usbserial_ls_throttle(struct tty_struct *tty)
{
	struct usb_serial_port *port;
	struct device *dev;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_throttle()");
	return usb_serial_generic_throttle(tty);
}
static void usbserial_ls_unthrottle(struct tty_struct *tty)
{
	struct usb_serial_port *port;
	struct device *dev;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_unthrottle()");
	return usb_serial_generic_unthrottle(tty);
}
static int  usbserial_ls_tiocmget(struct tty_struct *tty)
{
	struct usb_serial_port *port;
	struct device *dev;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_tiocmget()");
	// return usb_serial_generic_tiocmget(tty);
	return 0;
}
static int  usbserial_ls_tiocmset(struct tty_struct *tty, unsigned int set, unsigned int clear)
{
	struct usb_serial_port *port;
	struct device *dev;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_tiocmset()");
	// return usb_serial_generic_tiocmset(tty, set, clear);
	return 0;
}
static int  usbserial_ls_tiocmiwait(struct tty_struct *tty, unsigned long arg)
{
	struct usb_serial_port *port;
	struct device *dev;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_tiocmiwait()");
	return usb_serial_generic_tiocmiwait(tty, arg);
}
static int  usbserial_ls_get_icount(struct tty_struct *tty, struct serial_icounter_struct *icount)
{
	struct usb_serial_port *port;
	struct device *dev;
	port = tty->driver_data;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_get_icount()");
	return usb_serial_generic_get_icount(tty, icount);
}
 */


	/* USB events */
static void usbserial_ls_read_int_callback(struct urb *urb)
{
	struct usb_serial_port *port;
	struct device *dev;
	char *ch;
	int i;
	ch = (char *)urb->transfer_buffer;
	port = urb->context;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_read_int_callback(); len: %i", urb->actual_length);

	if (!urb->actual_length)
		return;
	/*
	 * The per character mucking around with sysrq path it too slow for
	 * stuff like 3G modems, so shortcircuit it in the 99.9999999% of
	 * cases where the USB serial is not a console anyway.
	 */
	if (!port->port.console || !port->sysrq) {
		tty_insert_flip_string(&port->port, ch, urb->actual_length);
	} else {
		for (i = 0; i < urb->actual_length; i++, ch++) {
			if (!usb_serial_handle_sysrq_char(port, *ch))
				tty_insert_flip_char(&port->port, *ch, TTY_NORMAL);
		}
	}
	tty_flip_buffer_push(&port->port);
	//resubmit urb
	usb_submit_urb(urb, GFP_KERNEL);
}
static void usbserial_ls_write_int_callback(struct urb *urb)
{
	struct usb_serial_port *port;
	struct device *dev;
	char *ch;
	ch = (char *)urb->transfer_buffer;
	port = urb->context;
	dev = &port->dev;
	dev_dbg(dev, "usbserial_ls_write_int_callback(); fifo-len: %i", kfifo_len(&port->write_fifo));
	urb->transfer_buffer_length = kfifo_out_locked(&port->write_fifo, ch, port->interrupt_out_size, &port->lock);
	urb->actual_length = urb->transfer_buffer_length;
	dev_dbg(dev, "usbserial_ls_write_int_callback(); transfer_buffer_length: %i", urb->transfer_buffer_length);

	//resubmit urb
	usb_submit_urb(urb, GFP_KERNEL);
}

static struct usb_serial_driver usb_serial_generic_device = {
	.driver = {
		.owner =	THIS_MODULE,
		.name =		"generic_lowspeed",
	},
	.id_table =		id_table,
	.probe =		usbserial_ls_probe,
	.attach = 		usbserial_ls_attach,
	.calc_num_ports =	usbserial_ls_calc_num_ports,
	// .disconnect = 		usbserial_ls_disconnect,
	// .release = 			usbserial_ls_release,
	.port_probe = 		usbserial_ls_port_probe,
	// .port_remove =		usbserial_ls_port_remove,
	// .suspend = 			usbserial_ls_suspend,
	// .resume = 			usbserial_ls_resume,
	// .reset_resume = 	usbserial_ls_reset_resume,
	.open = 			usbserial_ls_open,
	.close = 			usbserial_ls_close,
	.write =			usbserial_ls_write,

	.write_room = 		usbserial_ls_write_room,
	.tx_empty = 		usbserial_ls_tx_empty,

	.read_int_callback = usbserial_ls_read_int_callback,
	.write_int_callback = usbserial_ls_write_int_callback,
};

static struct usb_serial_driver * const serial_drivers[] = {
	&usb_serial_generic_device, NULL
};


module_usb_serial_driver(serial_drivers, id_table)

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
