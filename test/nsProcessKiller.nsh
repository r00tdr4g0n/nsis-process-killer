!define nsProcessKiller::Test `!insertmacro nsProcessKiller::Test`

!macro nsProcessKiller::Test _STR
  nsProcessKiller::_Test /NOUNLOAD `${_STR}`
!macroend

!define nsProcessKiller::FindProcess `!insertmacro nsProcessKiller::FindProcess`

!macro nsProcessKiller::FindProcess _PROCESS
  nsProcessKiller::_FindProcess /NOUNLOAD `${_PROCESS}`
!macroend

!define nsProcessKiller::KillProcess `!insertmacro nsProcessKiller::KillProcess`

!macro nsProcessKiller::KillProcess _PID
  nsProcessKiller::_KillProcess /NOUNLOAD `${_PID}`
!macroend
