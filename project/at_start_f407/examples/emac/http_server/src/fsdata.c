#include "lwip/apps/fs.h"
#include "lwip/def.h"

#define file_NULL (struct fsdata_file *) NULL

#ifndef FS_FILE_FLAGS_HEADER_INCLUDED
#define FS_FILE_FLAGS_HEADER_INCLUDED 1
#endif
#ifndef FS_FILE_FLAGS_HEADER_PERSISTENT
#define FS_FILE_FLAGS_HEADER_PERSISTENT 0
#endif

/* FSDATA_FILE_ALIGNMENT: 0=off, 1=by variable, 2=by include */
#ifndef FSDATA_FILE_ALIGNMENT
#define FSDATA_FILE_ALIGNMENT 0
#endif
#ifndef FSDATA_ALIGN_PRE
#define FSDATA_ALIGN_PRE
#endif
#ifndef FSDATA_ALIGN_POST
#define FSDATA_ALIGN_POST
#endif
#if FSDATA_FILE_ALIGNMENT==2
#include "fsdata_alignment.h"
#endif
#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__img_sics_gif = 0;
#endif

const unsigned char main_page_name[] = "/AT32F407.html";
const unsigned char led_page_name[] = "/AT32F407LED.html";
const unsigned char adc_page_name[] = "/AT32F407ADC.html";

static const unsigned char FSDATA_ALIGN_PRE data_AT32F407_html[] FSDATA_ALIGN_POST = "\
HTTP/1.0 200 OK\r\n\
Content-Length: 9848\r\n\
Content-Type: text/html\r\n\r\n\
<html>\
    <head>\
        <link rel=\"shortcut icon\" href=\"#\" />\
        <title>Technology Corp. Demo Web Page</title>\
    </head>\
    <body>\
        <div style=\"text-align: center; margin-left: 40px; width: 977px;\">\
            <span>\
                <br />\
                <big>\
                    <big>\
                        <big style=\"font-weight: bold;\"><big>AT32F407 web server demo </big></big>\
                    </big>\
                </big>\
            </span>\
            <small>\
                <small>\
                    <big>\
                        <big>\
                            <big style=\"font-weight: bold;\">\
                                <big>\
                                    <font size=\"6\">\
                                        <big>\
                                            <big>\
                                                <big>\
                                                    <big>\
                                                        <big><strong></strong></big>\
                                                    </big>\
                                                </big>\
                                            </big>\
                                        </big>\
                                    </font>\
                                </big>\
                            </big>\
                        </big>\
                    </big>\
                    <font size=\"6\">\
                        <big>&nbsp;</big>\
                        <small style=\"font-family: Verdana;\">\
                            <small>\
                                <big>\
                                    <big>\
                                        <big>\
                                            <big style=\"font-weight: bold; color: rgb(51, 51, 255);\">\
                                                <big>\
                                                    <strong><span style=\"font-style: italic;\"></span></strong>\
                                                </big>\
                                                <span style=\"color: rgb(51, 51, 255);\">\
                                                    <font size=\"8\">\
                                                        <br />\
                                                        ****************************<br />\
                                                        ****************************\
                                                    </font>\
                                                </span>\
                                            </big>\
                                        </big>\
                                    </big>\
                                </big>\
                            </small>\
                        </small>\
                    </font>\
                </small>\
            </small>\
            <hr style=\"width: 100%; height: 2px;\" />\
            <table style=\"width: 976px; height: 60px;\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">\
                <tbody>\
                    <tr>\
                        <td style=\"font-family: Verdana; font-weight: bold; font-style: italic; background-color: rgb(51, 51, 255); width: 317px;\">\
                            <big>\
                                <big>\
                                    <a href=\"AT32F407.html\"><span style=\"color: red;\">Main Page</span></a>\
                                </big>\
                            </big>\
                        </td>\
                        <td style=\"font-family: Verdana; font-weight: bold; background-color: rgb(51, 51, 255); width: 317px;\">\
                            &nbsp;\
                            <big>\
                                <big>\
                                    <a href=\"AT32F407LED.html\"><span style=\"color: white;\">LED Control</span></a>\
                                </big>\
                            </big>\
                        </td>\
                        <td style=\"font-family: Verdana; font-weight: bold; background-color: rgb(51, 51, 255); width: 317px;\">\
                            <big>\
                                <big>\
                                    <a href=\"AT32F407ADC.html\"><span style=\"color: white;\">ADC Sampling</span></a>\
                                </big>\
                            </big>\
                        </td>\
                    </tr>\
                </tbody>\
            </table>\
            <table style=\"background-color: rgb(255, 255, 255); width: 766px; text-align: left; margin-left: auto; margin-right: auto;\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\
                <tbody>\
                    <tr valign=\"top\">\
                        <td class=\"text\">\
                            <table border=\"0\" cellpadding=\"3\" cellspacing=\"0\" width=\"100%\">\
                                <tbody>\
                                    <tr>\
                                        <td>\
                                            <table border=\"0\" cellpadding=\"3\" cellspacing=\"0\" width=\"100%\">\
                                                <tbody>\
                                                    <tr>\
                                                        <td valign=\"top\" width=\"72%\">\
                                                            <font size=\"6\"> </font>\
                                                            <p align=\"center\">\
                                                                <font size=\"6\">Board Resource: <br /> </font>\
                                                            </p>\
                                                            <li style=\"text-align: left; margin-left: 80px; font-family: Times New Roman;\">\
                                                                <big><big>Main Chip: AT32F407VCT7 (RAM:96KB; FLASH:256KB)</big></big>\
                                                            </li>\
                                                            <li style=\"text-align: left; margin-left: 80px; font-family: Times New Roman;\">\
                                                                <big>\
                                                                    <big>Communication I/O: <br /></big>\
                                                                </big>\
                                                            </li>\
                                                            <li style=\"text-align: left; margin-left: 120px; font-family: Times New Roman;\">\
                                                                <big><big>3 I2C</big></big>\
                                                            </li>\
                                                            <li style=\"text-align: left; margin-left: 120px; font-family: Times New Roman;\">\
                                                                <big><big>8 USART</big></big>\
                                                            </li>\
                                                            <li style=\"text-align: left; margin-left: 120px; font-family: Times New Roman;\">\
                                                                <big><big>4 SPI</big></big>\
                                                            </li>\
                                                            <li style=\"text-align: left; margin-left: 120px; font-family: Times New Roman;\">\
                                                                <big><big>2 CAN</big></big>\
                                                            </li>\
                                                            <li style=\"text-align: left; margin-left: 120px; font-family: Times New Roman;\">\
                                                                <big><big>Crystal-less USB 2.0</big></big>\
                                                            </li>\
                                                            <li style=\"text-align: left; margin-left: 120px;\">\
                                                                <big>\
                                                                    <big><span style=\"font-family: Times New Roman;\">2 SDIO</span><br /> </big>\
                                                                </big>\
                                                            </li>\
                                                            <font size=\"6\">\
                                                                <font size=\"5\">\
                                                                    <font size=\"6\">\
                                                                        <font color=\"red\">\
                                                                            <font color=\"black\">\
                                                                                <font size=\"5\">\
                                                                                    <font color=\"red\"><font color=\"black\"> </font> </font>\
                                                                                </font>\
                                                                            </font>\
                                                                        </font>\
                                                                    </font>\
                                                                </font>\
                                                            </font>\
                                                        </td>\
                                                    </tr>\
                                                </tbody>\
                                            </table>\
                                        </td>\
                                    </tr>\
                                </tbody>\
                            </table>\
                        </td>\
                    </tr>\
                </tbody>\
            </table>\
        </div>\
    </body>\
</html>\
"
;

static const unsigned char FSDATA_ALIGN_PRE data_AT32F407LED_html[] FSDATA_ALIGN_POST = "\
HTTP/1.0 200 OK\r\n\
Content-Length: 4612\r\n\
Content-Type: text/html\r\n\r\n\
<html>\
<head>\
    <link rel=\"shortcut icon\" href=\"#\" />\
    <title>Technology Corp. Demo Web Page</title>\
</head>\
<body>\
    <div style=\"text-align: center; margin-left: 40px; width: 977px;\">\
        <span><br>\
        <big><big><big style=\"font-weight: bold;\"><big>AT32F407 web server demo</big></big></big></big></span> <small><small><big><big><big style=\"font-weight: bold;\"><big><font size=\"6\"><big><big><big><big><big><strong></strong></big></big></big></big></big></font></big></big></big></big><font size=\"6\"><big>&nbsp;</big><small style=\"font-family: Verdana;\"><small><big><big><big><big style=\"font-weight: bold; color: rgb(51, 51, 255);\"><big><strong><span style=\"font-style: italic;\"></span></strong></big><span style=\"color: rgb(51, 51, 255);\"><font size=\"8\"><br>\
        ****************************<br>\
        ****************************</font></span></big></big></big></big></small></small></font></small></small>\
        <hr style=\"width: 100%; height: 2px;\">\
        <table border=\"1\" cellpadding=\"2\" cellspacing=\"2\" style=\"width: 976px; height: 60px;\">\
            <tbody>\
                <tr>\
                    <td style=\"font-family: Verdana; font-weight: bold; background-color: rgb(51, 51, 255); width: 316px;\"><big><big><a href=\"AT32F407.html\"><span style=\"color: white;\">Main Page</span></a></big></big></td>\
                    <td style=\"font-family: Verdana; font-weight: bold; font-style: italic; background-color: rgb(51, 51, 255); width: 317px;\">\
                        <a href=\"AT32F407LED.html\"><span style=\"color: red;\">&nbsp;<big><big>LED Control</big></big></span></a>\
                    </td>\
                    <td style=\"font-family: Verdana; font-weight: bold; background-color: rgb(51, 51, 255); width: 317px;\"><big><big><a href=\"AT32F407ADC.html\"><span style=\"color: white;\">ADC Sampling</span></a></big></big></td>\
                </tr>\
            </tbody>\
        </table>\
        <table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" style=\"background-color: rgb(255, 255, 255); width: 766px; text-align: left; margin-left: auto; margin-right: auto;\">\
            <tbody></tbody>\
        </table><br>\
        <span style=\"font-family: Consolas;\"><big><big>On this page, you can control 3 LEDs on developing board:</big></big></span> <span style=\"font-family: Verdana;\"></span>\
        <form action=\"method=get\">\
            <span style=\"font-family: Verdana;\"></span>\
            <div style=\"text-align: center;\">\
                <span style=\"font-family: Verdana;\"><font size=\"5\" style=\"font-family: Consolas;\"></font></span>\
                <div style=\"text-align: center; font-family: Consolas;\">\
                    <span style=\"font-family: Verdana;\"><font size=\"5\"><input name=\"led\" type=\"checkbox\" value=\"2\"> LED2</font><br></span>\
                </div><span style=\"font-family: Verdana;\"><font size=\"5\" style=\"font-family: Consolas;\"></font></span>\
                <div style=\"text-align: center; font-family: Consolas;\">\
                    <span style=\"font-family: Verdana;\"><font size=\"5\"><input name=\"led\" type=\"checkbox\" value=\"3\"> LED3</font><br></span>\
                </div><span style=\"font-family: Verdana;\"><font size=\"5\" style=\"font-family: Consolas;\"></font></span>\
                <div style=\"text-align: center; font-family: Consolas;\">\
                    <span style=\"font-family: Verdana;\"><font size=\"5\"><input name=\"led\" type=\"checkbox\" value=\"4\"> LED4</font><br></span>\
                </div><span style=\"font-family: Verdana;\"><font size=\"5\" style=\"font-family: Consolas;\"></font></span>\
            </div><span style=\"font-family: Verdana;\"><font size=\"5\" style=\"font-family: Arial Unicode MS;\"><font size=\"5\"><input type=\"submit\" value=\"Send Command\"></font></font></span>\
        </form><span style=\"font-family: Verdana;\"></span>\
        <h3><span style=\"font-family: Verdana;\"><font size=\"5\"><font size=\"5\"><font size=\"-1\"><span style=\"color: black;\"></span></font></font></font></span></h3>\
        <h3 style=\"font-family: Verdana;\"><font size=\"5\"><font size=\"5\"><font size=\"-1\"><br></font></font></font></h3><font size=\"5\"><font size=\"5\"><span style=\"font-family: SimSun;\"></span><a href=\"/AT32F407.html\" style=\"font-family: SimSun;\"><font size=\"-1\"><big><span style=\"font-weight: bold;\"></span></big></font></a> <font size=\"-1\"><span style=\"color: blue;\"><font size=\"5\"><span style=\"font-family: SimSun;\"><a href=\"http://item.taobao.com/item.htm?spm=0.0.0.0.nObYQP&amp;id=17524242055\" style=\"font-weight: bold;\"><font size=\"5\"><span style=\"font-family: SimSun;\"></span></font></a> <font size=\"5\"></font></span></font></span></font></font></font>\
    </div>\
</body>\
</html>\
"
;

static const unsigned char FSDATA_ALIGN_PRE data_AT32F407ADC_html[] FSDATA_ALIGN_POST = "\
HTTP/1.0 200 OK\r\n\
Content-Length: 4651\r\n\
Content-Type: text/html\r\n\r\n\
<html>\
<head>\
    <link rel=\"shortcut icon\" href=\"#\" />\
    <title>Technology Corp. Demo Web Page</title>\
    <meta content=\"1\" http-equiv=\"refresh\">\
</head>\
<body>\
    <div style=\"text-align: center; margin-left: 40px; width: 977px;\">\
        <span><br>\
        <big><big><big style=\"font-weight: bold;\"><big>AT32F407 web server demo</big></big></big></big></span> <small><small><big><big><big style=\"font-weight: bold;\"><big><font size=\"6\"><big><big><big><big><big><strong></strong></big></big></big></big></big></font></big></big></big></big><font size=\"6\"><big>&nbsp;</big><small style=\"font-family: Verdana;\"><small><big><big><big><big style=\"font-weight: bold; color: rgb(51, 51, 255);\"><big><strong><span style=\"font-style: italic;\"></span></strong></big><span style=\"color: rgb(51, 51, 255);\"><font size=\"8\"><br>\
        ****************************<br>\
        ****************************</font></span></big></big></big></big></small></small></font></small></small>\
        <hr style=\"height: 2px;\">\
        <table border=\"1\" cellpadding=\"2\" cellspacing=\"2\" style=\"width: 976px; height: 60px;\">\
            <tbody>\
                <tr>\
                    <td style=\"font-family: Verdana; font-weight: bold; background-color: rgb(51, 51, 255); width: 316px;\"><big><big><a href=\"AT32F407.html\"><span style=\"color: white;\">Main Page</span></a></big></big></td>\
                    <td style=\"font-family: Verdana; font-weight: bold; background-color: rgb(51, 51, 255); width: 316px;\">\
                        <a href=\"AT32F407LED.html\"><span style=\"color: white;\">&nbsp;<big><big>LED Control</big></big></span></a>\
                    </td>\
                    <td style=\"font-family: Verdana; font-weight: bold; font-style: italic; background-color: rgb(51, 51, 255); width: 316px;\"><big><big><a href=\"AT32F407ADC.html\"><span style=\"color: red;\">ADC Sampling</span></a></big></big></td>\
                </tr>\
            </tbody>\
        </table>\
        <table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" style=\"background-color: rgb(255, 255, 255); width: 766px; text-align: left; margin-left: auto; margin-right: auto;\">\
            <tbody></tbody>\
        </table>\
        <table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" style=\"background-color: rgb(255, 255, 255); width: 766px; text-align: left; margin-left: auto; margin-right: auto;\">\
            <tbody></tbody>\
        </table>\
        <p align=\"center\"><font size=\"6\">This page implement real-time sampling of voltage, ADC channel is 4<br></font> <font size=\"6\"><br></font></p>\
        <div style=\"text-align: center;\">\
            <big><span><big>Voltage:</big> <span style=\"color: red;\"><big>%.2fV</big></span></span></big>\
        </div>\
        <table border=\"3\" cellpadding=\"0\" cellspacing=\"0\" style=\"background-color: rgb(204, 204, 204); width: 520px; text-align: left; margin-left: auto; margin-right: auto;\">\
            <tbody>\
                <tr>\
                    <td>\
                        <div>\
                            <table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"%4d\">\
                                <tbody>\
                                    <tr>\
                                        <td bgcolor=\"#33FF00\" style=\"text-align: right;\">&nbsp;</td>\
                                    </tr>\
                                </tbody>\
                            </table>\
                        </div>\
                    </td>\
                </tr>\
            </tbody>\
        </table>\
        <table border=\"0\" style=\"width: 520px; text-align: left; margin-left: auto; margin-right: auto;\">\
            <tbody>\
                <tr>\
                    <td style=\"color: rgb(51, 51, 153); font-family: Times New Roman;\" width=\"14\">0V</td>\
                    <td style=\"font-family: Times New Roman;\" width=\"14\">0.5V</td>\
                    <td style=\"font-family: Times New Roman;\" width=\"14\">1V</td>\
                    <td style=\"font-family: Times New Roman;\" width=\"14\">1.5V</td>\
                    <td style=\"font-family: Times New Roman;\" width=\"14\">2V</td>\
                    <td style=\"font-family: Times New Roman;\" width=\"14\">2.5V</td>\
                    <td style=\"width: 14px; font-family: Times New Roman;\">3V</td>\
                </tr>\
            </tbody>\
        </table><font size=\"6\"><font size=\"-1\"><span style=\"color: black;\"></span></font><font size=\"-1\"><span style=\"color: black;\"></span></font><font size=\"-1\"><span style=\"color: black;\"></span></font></font><a href=\"/AT32F407.html\" style=\"font-family: Verdana;\"><big><span style=\"font-weight: bold;\"></span></big></a><span style=\"color: blue;\"><br>\
        <font size=\"5\"><span style=""></span></font></span>\
    </div>\
</body>\
</html>\
"
;

const struct fsdata_file file_AT32F407_html[] = { {
file_NULL,
main_page_name,
data_AT32F407_html,
sizeof(data_AT32F407_html),
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT,
}};

const struct fsdata_file file_AT32F407ADC_html[] = { {
file_AT32F407_html,
adc_page_name,
data_AT32F407ADC_html,
sizeof(data_AT32F407ADC_html),
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT,
}};

const struct fsdata_file file_AT32F407LED_html[] = { {
file_AT32F407ADC_html,
led_page_name,
data_AT32F407LED_html,
sizeof(data_AT32F407LED_html),
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT,
}};

#define FS_ROOT file_AT32F407LED_html
#define FS_NUMFILES 3
