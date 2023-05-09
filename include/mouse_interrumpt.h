// mouse_interrumpt.h

#ifndef _MOUSE_INTERRUMPT_h
#define _MOUSE_INTERRUMPT_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include "pio_usb.h"
#include "Adafruit_TinyUSB.h"

// Pin D+ for host, D- = D+ + 1
#ifndef PIN_USB_HOST_DP
#define PIN_USB_HOST_DP 14
#endif

// Pin for enabling Host VBUS. comment out if not used
// #ifndef PIN_5V_EN
// #define PIN_5V_EN        18
// #endif

// #ifndef PIN_5V_EN_STATE
// #define PIN_5V_EN_STATE 25
// #endif

// Language ID: English
#define LANGUAGE_ID 0x0409

// USB Host object
Adafruit_USBH_Host USBHost;

// Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_MOUSE, 2, false);

// mouse setup
void mouse_setup();
void mouse_setup1();
void mouse_loop();
void mouse_loop1();

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] =
    {
        TUD_HID_REPORT_DESC_MOUSE()};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint

//------------- Low pass filter with Butterworth -------------//
// Butterworth low-pass filter coefficients

void filter_report(hid_mouse_report_t const *report);

//--------------------------------------------------------------------+
// Setup and Loop on Core0
//--------------------------------------------------------------------+
void mouse_setup()
{

    // usb_hid.begin();
    pinMode(25, OUTPUT);
}
void mouse_setup1()
{
   
    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.pin_dp = PIN_USB_HOST_DP;
    USBHost.configure_pio_usb(1, &pio_cfg);

    // run host stack on controller (rhport) 1
    // Note: For rp2040 pico-pio-usb, calling USBHost.begin() on core1 will have most of the
    // host bit-banging processing works done in core1 to free up core0 for other works
     
    USBHost.begin(1);
}
void mouse_loop()
{
    Serial.flush();
}
void mouse_loop1()
{
      
    USBHost.task();
    
}
extern "C"
{

    // Invoked when device with hid interface is mounted
    // Report descriptor is also available for use.
    // tuh_hid_parse_report_descriptor() can be used to parse common/simple enough
    // descriptor. Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE,
    // it will be skipped therefore report_desc = NULL, desc_len = 0
    void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len)
    {
        (void)desc_report;
        (void)desc_len;
        uint16_t vid, pid;
        tuh_vid_pid_get(dev_addr, &vid, &pid);

        Serial.printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);
        Serial.printf("VID = %04x, PID = %04x\r\n", vid, pid);

        uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);
        if (itf_protocol == HID_ITF_PROTOCOL_MOUSE)
        {
            Serial.printf("HID Mouse\r\n");
            if (!tuh_hid_receive_report(dev_addr, instance))
            {
                Serial.printf("Error: cannot request to receive report\r\n");
            }
        }
    }

    // Invoked when device with hid interface is un-mounted
    void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
    {
        Serial.printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
    }

    // Invoked when received report from device via interrupt endpoint
    void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len)
    {

        filter_report((hid_mouse_report_t const *)report);

        // continue to request to receive report
        if (!tuh_hid_receive_report(dev_addr, instance))
        {
            Serial.printf("Error: cannot request to receive report\r\n");
        }
    }

} // extern C

//--------------------------------------------------------------------+
// Low pass filter Functions
//--------------------------------------------------------------------+

void filter_report(hid_mouse_report_t const *report)
{
    if (report->x != 0)
    {
        digitalWrite(25, HIGH);
    }
    else
    {
        digitalWrite(25, LOW);
    }
    Serial.print("Mouse coordinates: ");
    Serial.print(report->x, DEC);
    Serial.print(", ");
    Serial.println(report->y, DEC);
    Serial.print("Mouse button: ");
    Serial.print(report->buttons, DEC);
    Serial.println("");
    Serial.print("Mouse wheel: ");
    Serial.print(report->wheel, DEC);
    Serial.println("");
    Serial.print("Mouse pan: ");
    Serial.print(report->pan, DEC);
    Serial.println("");
}

#endif
