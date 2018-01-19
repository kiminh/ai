#! /usr/bin/env python
# -*-coding:utf-8 -*-


import struct
import argparse
import random
import numpy as np


# Basic model parameters as external flags.
FLAGS = None


def get_label_class(radio, class_num):
    # watch_duration_class
    if radio >= 1:
        # print("dirty data or protect")
        radio = 0.9
    y = np.floor(class_num * (1 / (1 + np.exp(6 - 10 * radio))))
    if y < 0:
        y = 0
    return y


def records2binary(recordsfile,
                   dictfile,
                   watchedfile,
                   predictsfile,
                   watched_size):
    D = dict()
    # load dict
    for index, line in enumerate(open(dictfile, "r")):
        D[line.strip()] = index

    fwatched = open(watchedfile, "wb")
    fpredicts = open(predictsfile, "wb")

    for lineindex, line in enumerate(open(recordsfile, "r")):
        tokens = line.strip().split(' ')
        records = tokens[1:]  # skip __label__

        # generate binary records
        records_len = len(records)
        for w in xrange(1, records_len):
            if records[w] not in D:
                continue

            indexs = []
            boundary = random.randint(1, watched_size)
            for c in xrange(1, boundary + 1):
                if w-c >= 0 and records[w-c] in D:
                    indexs.append(D[records[w-c]])
            while len(indexs) < watched_size:
                indexs.append(0)
            for index in indexs:
                fwatched.write(struct.pack('<i', index))
            fpredicts.write(struct.pack('<i', D[records[w]]))
        if lineindex % 1000000 == 0:
            print("{} lines processed ...".format(lineindex))

    fwatched.close()
    fpredicts.close()


def records2binary_pctr(recordsfile, dictfile, watchedfile, predictsfile, watched_size, watched_ratio_file):
    D = dict()
    # load dict
    for index, line in enumerate(open(dictfile, "r")):
        D[line.strip()] = index

    fwatched = open(watchedfile, "wb")
    fpredicts = open(predictsfile, "wb")
    fwatched_ratio = open(watched_ratio_file, "r")

    for index, line in enumerate(open(recordsfile, "r")):
        watched_ratio_line = fwatched_ratio.readline()

        records = line.strip().split(' ')[1:]
        watched_ratios = watched_ratio_line.strip().split(' ')[1:]
        watched_ratios = map(float, watched_ratios)

        # generate binary records
        records_len = len(records)
        begin = 0
        while begin <= records_len - watched_size:
            cnt = 0
            indexs = []
            for item in records[begin:]:
                cnt += 1
                if item not in D:
                    continue

                indexs.append(D[item])
                if len(indexs) == watched_size:
                    break
            begin += cnt
            if len(indexs) == watched_size:
                # 写入 indexs
                for index in indexs:
                    fwatched.write(struct.pack('<i', index))

                ratio = watched_ratios[begin-1]
                label_class = get_label_class(ratio, FLAGS.class_num_pctr)
                fpredicts.write(struct.pack('<i', label_class))

    fwatched.close()
    fpredicts.close()


def binary2records(recordsfile, dictfile, watchedfile, predictsfile):
    D = dict()
    # load dict
    for index, line in enumerate(open(dictfile, "r")):
        D[index] = line.strip()

    watched_size = FLAGS.watched_size
    watched = np.fromfile(watchedfile, np.int32)
    predicts = np.fromfile(predictsfile, np.int32)

    assert (watched.shape[0] % watched_size == 0)
    nlines = watched.shape[0] / watched_size
    assert (nlines == predicts.shape[0])

    with open(recordsfile, 'w') as frecords:
        for x in xrange(nlines):
            for y in xrange(watched_size):
                offset = x * watched_size + y
                n = watched[offset]
                frecords.write(D[n])
                frecords.write(' ')
            offset = x
            n = predicts[offset]
            frecords.write(D[n])
            frecords.write('\n')


def main():
    if FLAGS.binary:
        binary2records(FLAGS.output_records,
                       FLAGS.input_dict_file,
                       FLAGS.input_watched,
                       FLAGS.input_predicts)
    else:
        if FLAGS.calculate_recall_inputs:
            print("calculate recall inputs ...")
	    records2binary(FLAGS.input_records,
                           FLAGS.input_dict_file, 
                           FLAGS.output_watched, 
                           FLAGS.output_predicts, 
                           FLAGS.watched_size)
        records2binary_pctr(FLAGS.input_records,
                            FLAGS.input_dict_file,
                            FLAGS.output_watched_pctr,
                            FLAGS.output_predicts_pctr,
                            FLAGS.watched_size_pctr,
                            FLAGS.input_watched_ratio_file)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--input_records',
        type=str,
        default='',
        help='records file generated by transform.py.'
    )

    parser.add_argument(
        '--output_records',
        type=str,
        default='',
        help='Output records file from binary form.'
    )

    parser.add_argument(
        '--output_watched',
        type=str,
        default='',
        help='Binary form of watched records.'
    )

    parser.add_argument(
        '--output_watched_pctr',
        type=str,
        default='',
        help='Binary form of watched records pctr.'
    )

    parser.add_argument(
        '--input_watched',
        type=str,
        default='',
        help='Binary form of watched records.'
    )

    parser.add_argument(
        '--output_predicts',
        type=str,
        default='',
        help='Binary form of predicts for watched records.'
    )

    parser.add_argument(
        '--output_predicts_pctr',
        type=str,
        default='output.fasttext.predicts.pctr',
        help='Binary form of predicts for watched records for pctr.'
    )

    parser.add_argument(
        '--input_predicts',
        type=str,
        default='',
        help='Binary form of predicts for watched records.'
    )

    parser.add_argument(
        '--input_dict_file',
        type=str,
        default='',
        help='Input dict file for records, generated by vec2binary.py.'
    )

    parser.add_argument(
        '--input_watched_ratio_file',
        type=str,
        default='',
        help='Input watched ratio file for PCTR.'
    )

    parser.add_argument(
        '--binary',
        type=bool,
        default=False,
        help='True: Convert binary form to text form.'
    )

    parser.add_argument(
        '--watched_size',
        type=int,
        default=20,
        help='Watched size.'
    )

    parser.add_argument(
        '--watched_size_pctr',
        type=int,
        default=20,
        help='Watched size for pctr.'
    )

    parser.add_argument(
        '--max_per_user',
        type=int,
        default=5,
        help='Max number of watched windows selected per user.'
    )

    parser.add_argument(
        '--class_num_pctr',
        type=int,
        default=10,
        help=''
    )

    parser.add_argument(
        '--calculate_recall_inputs',
        type=int,
        default=0,
        help='Wheather calculate recall inputs'
    )

    FLAGS, unparsed = parser.parse_known_args()
    main()