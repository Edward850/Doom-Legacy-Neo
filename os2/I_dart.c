
#include <stdio.h>
#include <string.h>

#include "i_os2.h"
#include "i_sound.h"
#include "qmus2mid.h"
#include "debug.h"

LONG APIENTRY DartEvent ( ULONG ulStatus,
                        PMCI_MIX_BUFFER  pBuffer,
                        ULONG  ulFlags  )

{
   if (ulFlags & MIX_STREAM_ERROR) {
      printf( "DartEvent: MIX_STREAM_ERROR");
   } else if (ulFlags == MIX_WRITE_COMPLETE) {
      //printf( "DartEvent: MIX_WRITE_COMPLETE");
      PlayDART( pmData);
         // create next buffer
      I_UpdateSound();

   } else {
      printf("DartEvent: ulFlags=%d\n", (int) ulFlags);
   }
   return( TRUE );
}

//
// display error code
//
void  MciError( int ulError )
{
   int   rc;
   char  szBuffer[ 256];

   strcpy( szBuffer, "");
   rc = mciGetErrorString( ulError, szBuffer, 256);
   printf( "MciError#%d:'%s'\n", ulError, szBuffer);
}


//
// init audio device and DART
//
void  InitDART( PWINDATA dartData)
{
   MCI_AMP_OPEN_PARMS   AmpOpenParms;
   int   i, rc;

   dartData->usDartID = 0;
      // open audio device
   memset( &AmpOpenParms, 0, sizeof(MCI_AMP_OPEN_PARMS));
   AmpOpenParms.usDeviceID = (USHORT) 0;
   AmpOpenParms.pszDeviceType = (PSZ) MCI_DEVTYPE_AUDIO_AMPMIX;

   rc = mciSendCommand( 0,
                       MCI_OPEN,
                       MCI_WAIT | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
                       ( PVOID ) &AmpOpenParms,
                       0 );

   if (rc != MCIERR_SUCCESS)  {
      MciError( rc );
      return;
   }
   dartData->usDartID = AmpOpenParms.usDeviceID;
   printf( "InitDart: audio handle=%x\n", dartData->usDartID);

      // setup the mixer
   memset ( &dartData->MixSetupParms, 0, sizeof(MCI_MIXSETUP_PARMS));
   dartData->MixSetupParms.ulBitsPerSample = BPS_16;
   dartData->MixSetupParms.ulSamplesPerSec = HZ_11025;
   dartData->MixSetupParms.ulFormatTag = MCI_WAVE_FORMAT_PCM;
   dartData->MixSetupParms.ulChannels = CH_2;  // stereo

   /* Setup the mixer for playback of wave data
    */
   dartData->MixSetupParms.ulFormatMode = MCI_PLAY;
   dartData->MixSetupParms.ulDeviceType = MCI_DEVTYPE_WAVEFORM_AUDIO;
   dartData->MixSetupParms.pmixEvent    = DartEvent;

   rc = mciSendCommand( dartData->usDartID,
                        MCI_MIXSETUP,
                        MCI_WAIT | MCI_MIXSETUP_INIT,
                        ( PVOID ) &dartData->MixSetupParms,
                        0 );
   if ( rc != MCIERR_SUCCESS ) {
      MciError( rc );
      return;
   }

      // Set up the BufferParms data structure and allocate
      // device buffers from the Amp-Mixer
      // use 2 buffer
      // use 1024 bytes for Doom/2
   printf( "InitDart: suggested buffer size =%d\n", dartData->MixSetupParms.ulBufferSize);
   dartData->BufferParms.ulNumBuffers = MAX_BUFFERS; //dartData->MixSetupParms.ulNumBuffers;
   dartData->BufferParms.ulBufferSize = 4096;
   //dartData->BufferParms.ulBufferSize = dartData->MixSetupParms.ulBufferSize;
   dartData->BufferParms.pBufList     = dartData->MixBuffers;

   rc = mciSendCommand( dartData->usDartID,
                        MCI_BUFFER,
                        MCI_WAIT | MCI_ALLOCATE_MEMORY,
                        (PVOID) &dartData->BufferParms,
                        0 );
   if ( ULONG_LOWD( rc) != MCIERR_SUCCESS ) {
      MciError( rc );
      return;
   }
   printf( "InitDART: #%d buffers of length %d\n",
           (int) dartData->BufferParms.ulNumBuffers,
           (int) dartData->BufferParms.ulBufferSize);

      // Fill all device buffers with data from the audio file.
   for( i=0; i<dartData->BufferParms.ulNumBuffers; i++) {
      dartData->MixBuffers[ i].ulStructLength = sizeof( MCI_MIX_BUFFER);
      memset( dartData->MixBuffers[ i].pBuffer, 0, dartData->BufferParms.ulBufferSize);
      dartData->MixBuffers[ i].ulBufferLength = dartData->BufferParms.ulBufferSize;
   }

   dartData->MixBuffers[MAX_BUFFERS - 1].ulFlags = MIX_BUFFER_EOS;

      // Write two buffers to kick off the amp mixer.
   rc = dartData->MixSetupParms.pmixWrite( dartData->MixSetupParms.ulMixHandle,
                               dartData->MixBuffers,
                               dartData->BufferParms.ulNumBuffers);
   if (rc != MCIERR_SUCCESS)
      MciError( rc );

   printf( "InitDART: DONE\n");
}

//
// close audio access
//
void  ShutdownDART( PWINDATA dartData)
{
   int   rc;
   MCI_GENERIC_PARMS    GenericParms;

   if (dartData->usDartID == 0)            // audio not enabled?
      return;

   rc = mciSendCommand( dartData->usDartID,
                        MCI_BUFFER,
                        MCI_WAIT | MCI_DEALLOCATE_MEMORY,
                        (PVOID) &dartData->BufferParms,
                        0 );
   if ( rc != MCIERR_SUCCESS)
      MciError( rc );

   rc = mciSendCommand( dartData->usDartID,
                        MCI_CLOSE,
                        MCI_WAIT,
                        (PVOID) &GenericParms,
                        0 );
   if ( rc != MCIERR_SUCCESS)
      MciError( rc );

   dartData->usDartID = 0;

   printf( "ShutdownDART done!\n");
}

//
// play buffer
//
void  PlayDART( PWINDATA dartData)
{
   if (dartData->usDartID == 0)            // audio not enabled?
      return;

      // play current buffer
   dartData->MixSetupParms.pmixWrite(
                   dartData->MixSetupParms.ulMixHandle,
                   &dartData->MixBuffers[ dartData->PlayBuffer],
                   1);
   dartData->PlayBuffer++;              // play next buffer
   if (dartData->PlayBuffer >= dartData->BufferParms.ulNumBuffers)
      dartData->PlayBuffer = 0;         // start from beginning
}

//
// convert .mus files to midi and open midi device
//
int  RegisterMIDI( PWINDATA midiData, void* data, int len)
{
   FILE* blah;
/*
   //return qmus2mid( data, len, 0, 89, 64, 1);
   blah = fopen( "doom.mus", "wb");
   fwrite( data, len, 1, blah);
   fclose( blah);
   qmus2mid( "doom.mus", "doom.mid", 0, 89,64,1);

   OpenMIDI( midiData);
*/
   return 1;
}


//
// open midi device
//
void OpenMIDI( PWINDATA midiData)
{
   ULONG  rc;
   MCI_OPEN_PARMS mop;

   printf( "OpenMidi\n");
      // send MCI_OPEN
   memset( &mop, 0, sizeof( mop));
   mop.pszElementName = "doom.mid";
   rc = mciSendCommand( 0,
                       MCI_OPEN,
                       MCI_WAIT | MCI_OPEN_ELEMENT | MCI_OPEN_SHAREABLE,
                       ( PVOID ) &mop,
                       0 );
   if ( rc != MCIERR_SUCCESS) {
      MciError( rc );
      return;
   }
   midiData->usMidiID = mop.usDeviceID;  // save device id

   printf( "OpenMidi: %d\n", mop.usDeviceID);
}

//
// play midi file
//
int  PlayMIDI( PWINDATA midiData, int looping)
{
   ULONG  rc;
   MCI_PLAY_PARMS    PlayParms;

   printf( "PlayMidi\n");

   if (midiData->usMidiID == 0)            // midi not enabled?
      return 0;

      // save loop flag
   midiData->looping = looping;

      // set volume
   SetMIDIVolume( midiData, midiData->midiVolume);

   memset( &PlayParms, 0, sizeof( PlayParms));

   PlayParms.hwndCallback = midiData->hwndClient;

   rc = mciSendCommand( midiData->usMidiID,
                        MCI_PLAY,
                        MCI_NOTIFY,      // notify end of play action
                        (PVOID) &PlayParms,
                        0 );

   if ( rc != MCIERR_SUCCESS)
      MciError( rc );

   printf( "PlayMidi DONE\n");

   return 1;
}


void    PauseMIDI( PWINDATA midiData)
{
   ULONG rc;
   MCI_GENERIC_PARMS    GenericParms;

   if (midiData->usMidiID == 0)            // midi not enabled?
      return;

   printf( "PauseMidi\n");
   rc = mciSendCommand( midiData->usMidiID,
                        MCI_PAUSE,
                        0,//MCI_WAIT,
                        (PVOID) &GenericParms,
                        0 );
   if ( rc != MCIERR_SUCCESS)
      MciError( rc );
}


void    ResumeMIDI( PWINDATA midiData)
{
   ULONG rc;
   MCI_GENERIC_PARMS    GenericParms;

   if (midiData->usMidiID == 0)            // midi not enabled?
      return;

   printf( "ResumeMidi\n");
   rc = mciSendCommand( midiData->usMidiID,
                        MCI_RESUME,
                        0,//MCI_WAIT,
                        (PVOID) &GenericParms,
                        0 );
   if ( rc != MCIERR_SUCCESS)
      MciError( rc );
}

//
// closes midi device
//
void    ShutdownMIDI( PWINDATA midiData)
{
   ULONG rc;
   MCI_GENERIC_PARMS    GenericParms;

   printf( "ShutdownMIDI\n");

   if (midiData->usMidiID == 0)            // midi not enabled?
      return;

   rc = mciSendCommand( midiData->usMidiID,
                        MCI_CLOSE,
                        0,//MCI_WAIT,
                        (PVOID) &GenericParms,
                        0 );
   if ( rc != MCIERR_SUCCESS)
      MciError( rc );
   midiData->usMidiID = 0;
}

//
// set midi volume
//
void    SetMIDIVolume( PWINDATA midiData, int vol)
{
   ULONG rc;
   MCI_SET_PARMS  msp;

   if (midiData->usMidiID == 0)            // midi not enabled?
      return;

   printf( "SetMIDIVolume %d\n", vol);
   midiData->midiVolume = vol;
   msp.ulLevel = vol * 100 / 31;
   msp.ulAudio = MCI_SET_AUDIO_ALL;
   rc = mciSendCommand( midiData->usMidiID,
                        MCI_SET,
                        MCI_SET_AUDIO | MCI_SET_VOLUME, //MCI_WAIT |
                        (PVOID) &msp, 0);
   if ( rc != MCIERR_SUCCESS)
      MciError( rc );

}
