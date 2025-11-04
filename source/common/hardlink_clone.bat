@echo off
setlocal enabledelayedexpansion

::------------------------------------------------------------
:: 使い方:
::    hardlink_clone.bat <元フォルダ> <複製先フォルダ>
::------------------------------------------------------------

if "%~1"=="" (
	echo [ERROR] 元フォルダを指定してください。
	exit /b 1
)
if "%~2"=="" (
	echo [ERROR] 複製先フォルダを指定してください。
	exit /b 1
)

:: 絶対パスに変換
for %%I in ("%~1") do set "SRC=%%~fI"
for %%I in ("%~2") do set "DST=%%~fI"

:: 末尾のバックスラッシュ削除
if "%SRC:~-1%"=="\" set "SRC=%SRC:~0,-1%"
if "%DST:~-1%"=="\" set "DST=%DST:~0,-1%"

echo.
echo [INFO] ソース: %SRC%
echo [INFO] ターゲット: %DST%
echo.

:: 同一ドライブかチェック（ハードリンクは同一ボリューム内のみ有効）
if /i "%SRC:~0,2%" neq "%DST:~0,2%" (
	echo [ERROR] ハードリンクは異なるドライブ間では作成できません。
	exit /b 1
)

:: フォルダ構造を再帰的に複製
echo [STEP 1] フォルダ構造を複製中...
for /f "delims=" %%D in ('robocopy "%SRC%" "%DST%" /e /xf * /ndl /nfl /njh /njs /ns /nc') do (
	rem robocopyを使ってフォルダ構造のみ複製
)

:: 各ファイルを再帰的に処理してハードリンク作成
echo [STEP 2] ファイルをハードリンクとして複製中...
for /r "%SRC%" %%F in (*) do (
	set "REL=%%F"
	set "REL=!REL:%SRC%=!"
	set "TARGET=%DST%!REL!"

	:: 必要ならフォルダ作成（念のため）
	for %%P in ("!TARGET!") do if not exist "%%~dpP" mkdir "%%~dpP" >nul 2>&1

	if not exist "!TARGET!" (
		mklink /H "!TARGET!" "%%F" >nul 2>&1
		if errorlevel 1 (
			echo [WARN] ハードリンク作成失敗: %%F
		) else (
			echo [OK]   %%F
		)
	)
)

echo.
echo [DONE] 完了しました。
endlocal
exit /b 0
