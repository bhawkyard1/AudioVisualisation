import struct
import wave

w = wave.open('soviet_national_anthem_0.wav', 'r')
f = open('audiodat.txt', 'w')

for i in range(0, w.getnframes()):
	frame = w.readframes(1)
        
	data = struct.unpack("<i", frame)

	print data[0]
	
	f.write( str(data[0]) + '\n' )
	
f.close()
	
	
