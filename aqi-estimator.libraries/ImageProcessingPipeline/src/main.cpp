#include "program.h"

int main(int argc, char **args)
{
    #ifdef IMAGE_TO_BYTE_ARRAY
    process_image_into_byte_array(argc, args);
    #endif
    
    #ifdef BYTE_ARRAY_TO_ENHANCED_METADATA
    process_byteArray_to_enhanced_metadata(argc, args);
    #endif

    #ifdef CSV_TO_ENHANCED_METADATA
    process_CSV_to_enhanced_metadata(argc, args);
    #endif

    return 0;
}