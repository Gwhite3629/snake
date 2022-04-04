#ifndef _BITMAP_H_
#define _BITMAP_H_
typedef char* bitmap_t;
#define MAP_SIZE(n) (n/8)+(n&7)
#define set_bit(words, n) (words[n/8] |= ((char)1 << (n & (char)7)))
#define clear_bit(words, n) (words[n/8] &= ~((char)1 << (n & (char)7)))
#define get_bit(words, n) ((words[n/8] & ((char)1 << (n & (char)7))))
#endif