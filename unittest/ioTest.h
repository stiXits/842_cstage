bool run_IoTests();
bool test_extractOpcode_noOffset();
bool test_extractOpcode_offset();
bool test_extractOpcode_overlapping();

bool test_appendUncompressedByte_noOffset_lsB();
bool test_appendUncompressedByte_offset();
bool test_appendUncompressedByte_offset_msB();

bool test_readNextCompressedByte_noOffset();
bool test_readNextCompressedByte_offset();

bool test_readNextCompressedChunk_noOffset();
bool test_readNextCompressedChunk_offset();

bool test_appendCompressedChunk_offset();
