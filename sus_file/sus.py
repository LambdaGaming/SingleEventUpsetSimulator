import os
import random

print( "Single-event Upset Simulator (File Edition) | Copyright (c) 2023 LambdaGaming" )
print( "WARNING!!! THIS PROGRAM EDITS RANDOM BITS OF RANDOM FILES, WHICH CAN SERIOUSLY SCREW UP YOUR SYSTEM! PROCEED AT YOUR OWN RISK!" )
paths = []
numBytes = input( "Enter the amount of random bits you want to flip: " )
selectedDir = input( "Enter the directory you want to pick files from (Make sure the files have write permissions set!): " )

randFile = random.choice( [x for x in os.listdir( selectedDir ) if os.path.isfile( os.path.join( selectedDir, x ) )] )
finalPath = os.path.join( selectedDir, randFile )
print( f"Random file: {finalPath}" )
with open( finalPath, "rb" ) as file:
	readByte = bytearray( file.read() )
	for i in range( int( numBytes ) ):
		randOffset = random.randint( 0, len( readByte ) - 1 )
		randBit = random.randint( 0, 7 )
		print( f"Old data at offset {randOffset}: {readByte[randOffset]}" )
		readByte[randOffset] ^= ( 1 << randBit )
		print( f"New data at offset {randOffset}: {readByte[randOffset]}" )
with open( finalPath, "wb" ) as file:
	file.write( readByte )
