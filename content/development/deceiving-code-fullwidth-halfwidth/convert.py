#-*- coding: utf-8 -*-

from __future__ import unicode_literals
from __future__ import print_function

def convert_full_to_half_char(ch):
    codeval = ord(ch)
    if 0xFF00 <= codeval <= 0xFF5E:
        ascii = codeval - 0xFF00 + 0x20;
        return unichr(ascii)
    else:
        return ch

def convert_full_to_half_string(line):
    output_list = [convert_full_to_half_char(x) for x in line]
    return ''.join(output_list)

input_str = """！＂＃＄％＆＇（）＊＋，－．／
０１２３４５６７８９：；＜＝＞？
＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯ
ＰＱＲＳＴＵＶＷＸＹＺ［＼］＾＿
｀ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏ
ｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～"""
output_str = convert_full_to_half_string(input_str)
print(input_str)
print(output_str)
