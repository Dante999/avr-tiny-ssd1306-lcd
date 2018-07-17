#include "ssd1306.h"
#include "USI_TWI_Master.h"
#include <avr/io.h>
#include <string.h>

/******************************************************************************
 * settings for the ssd1306 display
******************************************************************************/

// the I2C address of the display
#define SSD1306_ADDR                0x78

// the width of the lcd in pixels
#define SSD1306_LCDWIDTH            128

// the height of the lcd in pixels
#define SSD1306_LCDHEIGHT           64


/******************************************************************************
 * ssd1306 specific i2c commands
 * 
 * You should not change any of these!
******************************************************************************/
#define SSD1306_SETCONTRAST         0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON        0xA5
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_INVERTDISPLAY       0xA7
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_DISPLAYON           0xAF
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETVCOMDETECT       0xDB
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_SETLOWCOLUMN        0x00
#define SSD1306_SETHIGHCOLUMN       0x10
#define SSD1306_SETSTARTLINE        0x40
#define SSD1306_MEMORYMODE          0x20
#define SSD1306_COLUMNADDR          0x21
#define SSD1306_PAGEADDR            0x22
#define SSD1306_COMSCANINC          0xC0
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SEGREMAP            0xA0
#define SSD1306_CHARGEPUMP          0x8D
#define SSD1306_EXTERNALVCC         0x1
#define SSD1306_SWITCHCAPVCC        0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL                         0x2F
#define SSD1306_DEACTIVATE_SCROLL                       0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL                 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL                  0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL    0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL     0x2A


#define LCD_BUFFER_SIZE         (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8)
#define BLOCK_SIZE              8


/*******************************************************************************
 * @brief   initialize the display
 *
 * @param   none
 *
 * @return  none
*******************************************************************************/
void ssd1306_init() {

    USI_TWI_Master_Initialise();

    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE

    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80

    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8

    ssd1306_command(64-1);

    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset

    ssd1306_command(SSD1306_SETSTARTLINE);// | 0x0);        // line #0

    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    ssd1306_command(0x14);                                  // using internal VCC

    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x00 horizontal addressing

    ssd1306_command(SSD1306_SEGREMAP | 0x1);                // rotate screen 180

    ssd1306_command(SSD1306_COMSCANDEC);                    // rotate screen 180

    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x12);

    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(0xCF);

    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    ssd1306_command(0xF1);

    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);

    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4

    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

    ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

    ssd1306_command(SSD1306_DISPLAYON);                     //switch on OLED

    ssd1306_fill(0x00);                                     // clear the whole display

}


/******************************************************************************
 * @brief   fills the whole display with the given symbol
 *
 * @param   symbol      the symbol which should fill the display (0x00 = off)
 *
 * @return  none
******************************************************************************/
void ssd1306_fill(uint8_t symbol) {
    unsigned char buffer[BLOCK_SIZE];
    memset(buffer, symbol, sizeof(buffer));

    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(0);
    ssd1306_command(SSD1306_LCDWIDTH-1);

    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(0);
    ssd1306_command(7);


   for( uint16_t i=0; i <= LCD_BUFFER_SIZE; i = i+BLOCK_SIZE) {
        ssd1306_data_block(buffer, BLOCK_SIZE);
   }
}


/******************************************************************************
 * @brief   sends the given command-byte to the display
 *
 * @param   none
 *
 * @return  none
******************************************************************************/
void ssd1306_command(uint8_t cmd) {
    static unsigned char buffer[3];

    buffer[0] = SSD1306_ADDR;
    buffer[1] = 0x00;
    buffer[2] = cmd;

    USI_TWI_Start_Transceiver_With_Data(buffer, 3);
}


/******************************************************************************
 * @brief   sends the given  data-byte to the display
 *
 * @param   none
 *
 * @return  none
******************************************************************************/
void ssd1306_data(uint8_t data) {
    static unsigned char buffer[3];

    buffer[0] = SSD1306_ADDR;
    buffer[1] = 0x40;
    buffer[2] = data;

    USI_TWI_Start_Transceiver_With_Data(buffer, 3);
}


/******************************************************************************
 * @brief   sends the given  data-byte to the display
 *
 * @param   none
 *
 * @return  none
******************************************************************************/
void ssd1306_data_block(uint8_t *data, uint8_t size) {

    size_t buffer_size = size + 2;

    uint8_t buffer[buffer_size];

    buffer[0] = SSD1306_ADDR;
    buffer[1] = 0x40;


    uint8_t *start = (uint8_t*) &buffer[2];

    memcpy(start, data, size);

    USI_TWI_Start_Transceiver_With_Data(buffer, buffer_size);

}


/******************************************************************************
 * @brief   draws the given tile to the display
 *
 * @param   page    the page where the tile should be placed (0 to 7)
 *                  where 0 is on the bottom and 7 on the top
 *
 * @param   column  the column where the tile should be placed (0 to 127)
 *
 * @param   tile    the tile which should be displayed (8x8 bits - uint8_t[8])
 *
 * @return  none
******************************************************************************/
void ssd1306_draw_tile(uint8_t page, uint8_t column, uint8_t *tile) {

    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(column);
    ssd1306_command(column+7);

    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(page);
    ssd1306_command(page);

    ssd1306_data_block(tile, 8);
}



