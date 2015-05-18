#ifndef ___cSpeedController_h___
#define ___cSpeedController_h___

#include <cstdint>

class cSpeedController
{
public:
   cSpeedController(uint32_t a_Level);
   ~cSpeedController();

   uint32_t GetMiliSecPerFall();
   void Update(uint32_t a_ElapsedMiliSec);

private:
   uint32_t m_StartingMsPerFall;
   uint32_t m_FastestMsPerFall;
   uint32_t m_CurrentMsPerFall;

   // After this many miliseconds m_CurrentMsPerFall will count down 1
   uint32_t m_ChangeRate;
   uint32_t m_Accumulator;
};

#endif
