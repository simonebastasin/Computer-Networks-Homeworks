# Computer Networks course

*(full text of each assignment is available in its folder)*

## Project

Consider a mono digital audio signal, in PCM format, which is assumed to be generated in real time. The transmitter performs signal packetization, with packets of 160 audio samples (corresponding to an audio interval of 20ms).<br/>
We want to design a speech recognition algorithm (Voice Activity Detection - VAD) which determines which packets have speech content and, therefore, must be transmitted, and which instead can be suppressed as they lack significant speech content. Note that packets with no speech content may still contain background noise. For each audio packet, the VAD algorithm will have to output the value 1 if the packet is to be transmitted, and 0 otherwise. The decision of the VAD algorithm regarding the n-th packet can be based on the signal samples of all previous packets k ≤ n and, potentially, of subsequent samples as well, as long as the overall encoding delay remains below 50 ms.<br/>
The goal is to transmit the smallest possible number of packets that allows the reconstruction of the speech signal without artifacts (clipping).

## Homework 1

Create the UTF8_encoding and UTF8_decoding scripts that respectively encode and decode according to the UTF-8 standard.