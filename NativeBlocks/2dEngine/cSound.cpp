#include "cSound.h"

cSound::cSound()
   :m_pSoundBuffer(NULL),
    m_PoolSize(1),
    m_SoundChannels(),
    m_Volume(100)
{
}

cSound::~cSound()
{
}

void cSound::SetBuffer(std::shared_ptr<sf::SoundBuffer> a_pSoundBuffer)
{
   m_pSoundBuffer = a_pSoundBuffer;
}

void cSound::Play()
{
   // If the source buffer is NULL don't do anything.
   if (m_pSoundBuffer == NULL)
   {
      return;
   }

   // Search through the pool and play a sound that isn't active
   for (sf::Sound& l_rSound : m_SoundChannels)
   {
      if (l_rSound.getStatus() == sf::SoundSource::Stopped)
      {
         l_rSound.play();
         return;
      }
   }

   // Didn't find any available sounds in the pool. If we haven't reached the
   // max pool size then create a new sound.
   if (m_SoundChannels.size() < m_PoolSize)
   {
      sf::Sound l_NewSound(*(m_pSoundBuffer.get()));
      m_SoundChannels.push_back(l_NewSound);
      m_SoundChannels.back().setVolume(m_Volume);
      m_SoundChannels.back().play();
      return;
   }

   // This is the worst case. We don't have any available sounds in the pool to
   // play. Stop the first item short and play it again.
   m_SoundChannels.front().play();
}

void cSound::SetPoolSize(uint32_t a_PoolSize)
{
   m_PoolSize = a_PoolSize;
}

void cSound::SetVolume(double a_VolumePercent)
{
   m_Volume = a_VolumePercent;
   for (sf::Sound& l_rSound : m_SoundChannels)
   {
      l_rSound.setVolume(a_VolumePercent);
   }
}
