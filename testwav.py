import struct
import wave

w = wave.open('silence.wav', 'r')
for i in range(w.getnframes()):
    ### read 1 frame and the position will updated ###
    frame = w.readframes(1)
    
    unpacked_signed_value = struct.unpack("<h", frame)

    print str(unpacked_signed_value)
