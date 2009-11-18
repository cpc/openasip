#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Calculates, how many points denbench run did get.

Normalise cyclecounts to level of 1.1 and calculates how much better or worse
results were compared to compare_version values.

Weight is estimated by number how many similair testcases are

If there is two data sets for the same algorithm weight is 0.5 for
each of the cases. If algorith is quite similair but not exactly
then weight is a bit bigger.

author Mikael Lepistö 2009
"""

compare_version = "tce-1.1 rev 532"
normalize_cycles_and_weight = dict(aes=(3261322550, 1.0),
                                   cjpegv2data1=(18142671, 0.6),
                                   djpegv2data1=(25810024, 0.6),
                                   huffde=(13103021, 1.0),
                                   mp2decodf32psnr1=(6834565268,0.5),
                                   mp2decodfixpsnr1=(6840529582,0.5),
                                   mp2enf32data1=(36890093513, 0.5),
                                   mp2enfixdata1=(6003220082, 0.5),
                                   mp3playerfixeddata1=(2317445676,1.0),
                                   mp4decodepsnr1=(7840120307,1.0),
                                   mp4encodedata1=(1681980123,1.0),
                                   rgbcmykv2data1=(83226370,0.4),
                                   rgbhpgv2data1=(14506342,0.4),
                                   rgbyiqv2data1=(35442344,0.4))

# collect cycles for each test case from stdin
normalized_weighted_results = dict()
import sys

# calculate goodness
goodness = 0.0
weight_sum = 0.0
print "%20s%10s,%10s" % ("case","compared","weighted")
for line in sys.stdin:
    try: 
        case,result = line.strip().split(' ')
    except:
        break
    compare_cycles,weight = normalize_cycles_and_weight[case]
    result = float(compare_cycles)/float(result)
    weighted_result = result*weight
    weight_sum += weight
    print "%20s%10f%10f" %(case,result,weighted_result)
    goodness += weighted_result
    
print "Overall goodness points compared to %s is %f" % (compare_version,
                                                        goodness/weight_sum)
