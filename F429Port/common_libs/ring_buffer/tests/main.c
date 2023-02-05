#include "../ring_buffer.h"
#include <stdio.h>
#include <string.h>

#define PRINT_RB_PARAMS(p_rb) \
    printf("Front           %d\r\n", (p_rb)->front); \
    printf("Back            %d\r\n", (p_rb)->back); \
    printf("Bytes Available %d\r\n", (p_rb)->bytes_available); \
    printf("Data Map:\r\n"); \
    for (uint32_t iter = 0; iter < RING_BUFFER_SIZE; iter++){ \
        printf("0x%X ", (p_rb)->buffer[iter]); \
    } \
    printf("\r\n");

#define PRINT_RX_BUFFER(size) \
    printf("Received Data Map:\r\n"); \
    for (uint32_t iter = 0; iter < size; iter++){ \
        printf("0x%X ", buffer_4[iter]); \
    } \
    printf("\r\n");

ring_buffer_t rb;

uint8_t buffer_1[31];
uint8_t buffer_2[12];
uint8_t buffer_3[12];
uint8_t buffer_4[32];

uint8_t ping_pack[125] = { 0xA5, 0x5A, 0x01, 0x01, 0x00, 0x00, 0x00, 'A', 'P', 'P', '1' };
uint8_t get_pack[125];

int main(){
    // memset(buffer_1, 0xBB, 31);
    // memset(buffer_2, 0xCC, 1);
    // memset(buffer_3, 0xDD, 32);

    // for (uint8_t iter = 0; iter < 125; iter++)
        // ping_pack[iter] = iter;
// 
    rb_init(&rb);
    uint32_t missed_count = 0;
    for (uint32_t iter = 0; iter < 1000; iter++){
        /*printf("PUSH DATA 0x%X\r\n", */rb_push_data(&rb, ping_pack, 11);
        PRINT_RB_PARAMS(&rb);
        /*printf("TAKE DATA 0x%X\r\n", */rb_take_data(&rb, get_pack, 11);
        PRINT_RB_PARAMS(&rb);
        // printf("DATA TAKEN FROM QUEUE IS %d\r\n", memcmp(get_pack, ping_pack, 125));
        printf("Message Number %d\r\n", iter+1);
        printf("START 0x%X\r\nCMD 0x%X\r\nWSIZE 0x%X\r\nNAME 0x%X\r\n\r\n", *((uint16_t *)get_pack),
                                                                            get_pack[2],
                                                                            *((uint32_t *)(get_pack + 3)),
                                                                            *((uint32_t *)(get_pack + 7)));
        if (memcmp(ping_pack, get_pack, 11) != 0){
            // PRINT_RB_PARAMS(&rb);
            printf("ERROR OCCURED!!!\r\n");
            getchar();   
            missed_count++;
        }
        printf("----------------------------\r\n\r\n");
        // printf("CMD 0x%X WSIZE 0x%X NAME %.4s\r\n", get_pack[0], *((uint32_t *)(get_pack + 1)), get_pack + 5);
        // printf("-----------------------------------------\r\n");
    }

    printf("MISSED MESSAGES COUNT %d\r\n", missed_count);
    return 0;
}