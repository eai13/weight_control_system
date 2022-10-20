#ifndef APP2_MULTIPLYBLOCK_H
#define APP2_MULTIPLYBLOCK_H

#include <QtCore/qglobal.h>
#include "app2_customblock.h"
#include <math.h>

class APP2_mathoperationblock : public APP2_customblock{
public:
    enum BlockType{
        MATH_BLOCK_PRODUCT  = 0x01,
        MATH_BLOCK_SUM      = 0x02,
        MATH_BLOCK_DIF      = 0x03,
        MATH_BLOCK_DIV      = 0x04,
        MATH_BLOCK_POW      = 0x05
    };

    APP2_slotnode *     input_1 = nullptr;
    APP2_slotnode *     input_2 = nullptr;
    APP2_signalnode *   output = nullptr;

    float input_1_value = 0;
    float input_2_value = 0;
    float output_value = 0;

    APP2_mathoperationblock(QGraphicsScene * parent, BlockType bt);

    void ProcessBlockData(void) override;

private:
    BlockType block_type = MATH_BLOCK_SUM;
};

#endif // APP2_MULTIPLYBLOCK_H
