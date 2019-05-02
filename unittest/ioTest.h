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

bool test_appendCompressedChunk_noOffset();
bool test_appendCompressedChunk_offset();

bool test_appendOpcode_noOffset();
bool test_appendOpcode_offset();
bool test_appendOpcode_overlappingOffset();
bool test_appendOpcode_onlyLow();
bool test_appendOpcode_offsetLow();
bool test_appendCompressedChunk_onlyLow();
bool test_appendOpcodeAndChunk();

bool test_readCompressedChunk_noOffset();
bool test_readCompressedChunk_offset();
