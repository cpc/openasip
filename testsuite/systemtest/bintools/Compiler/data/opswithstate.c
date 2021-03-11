int main() {
#if SEQUENTIAL == 0
  // These should get scheduled parallel thanks to the different FUs.
  _TCEFU_STDOUT("OUT1", '1');
  _TCEFU_STDOUT("OUT2", '2');
#else
  // These should be forced serial
  _TCE_STDOUT('1');
  _TCE_STDOUT('2');
#endif
  return 0;
}

