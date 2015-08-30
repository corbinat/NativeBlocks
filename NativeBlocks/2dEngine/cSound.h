// This class holds sound effects. There is a limit to the number of sf::Sounds
// we can make, so this class limits the simultaneous channels.
// NOTE: This class doesn't make sure ALL different sounds are within a limit.
// It just limits this one sound to a certain pool size.

#ifndef ___cSound_h___
#define ___cSound_h___

#include "SFML/Audio.hpp"
#include <memory>

class cSound
{
public:
   cSound();
   ~cSound();

   void SetBuffer(std::shared_ptr<sf::SoundBuffer> a_pSoundBuffer);
   void Play();
   void SetPoolSize(uint32_t a_PoolSize);
   void SetVolume(double a_VolumePercent);


private:

   std::shared_ptr<sf::SoundBuffer> m_pSoundBuffer;

   uint32_t m_PoolSize;
   std::vector<sf::Sound> m_SoundChannels;
   double m_Volume;
};


#endif
