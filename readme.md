### 프로젝트 설명
#### nsProcessKiller
`NSIS Plug-in` 용 dll로 특정 프로세스 찾기 및 종료 기능을 제공
#### Target
위 dll 테스트를 위한 `Target.exe` 생성용 프로젝트로 특별한 기능 없음
<br/><br/><br/>

### 사용법
`test` 디렉토리의 파일들을 아래에서 특정한 위치로 이동
|파일명|위치|비고|
|---|---|---|
|nsProcessKiller.nsh|C:\Program Files (x86)\NSIS\Include||
|nsProcessKiller.nsh|C:\Program Files (x86)\NSIS\Plugins\x86-unicode||

#### 프로세스 찾기
- `Target.exe` 프로세스를 찾고, 찾은 경우 `PID`를 반환
- `PID`는 두 번째 명령으로 얻으며, `$0` 레지스터에 저장
```
${nsProcessKiller::FindProcess} "Target.exe"
Pop $0 
```
#### 프로세스 종료
- 프로세스 찾기에서 얻은 `PID($0에 저장)`를 사용하여 호출
```
${nsProcessKiller::KillProcess} $0
```