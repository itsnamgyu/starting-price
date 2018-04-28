#define CNAME "test.c"
#if CNAME == __FILE__
#error You did it!
#endif
// No, you can't do this
