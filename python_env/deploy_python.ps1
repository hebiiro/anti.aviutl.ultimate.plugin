#
# このファイルはPython環境を構築するPowerShellスクリプトです。
# このファイルが属するフォルダにpython32フォルダを作成し、その中にPython環境を構築します。
#

# 選択肢の作成
$typename = "System.Management.Automation.Host.ChoiceDescription"
$yes = new-object $typename("&Yes", "実行する")
$no  = new-object $typename("&No", "実行しない")

# 選択肢コレクションの作成
$assembly= $yes.getType().AssemblyQualifiedName
$choice = new-object "System.Collections.ObjectModel.Collection``1[[$assembly]]"
$choice.add($yes)
$choice.add($no)

# 選択プロンプトの表示
$answer = $host.ui.PromptForChoice("このフォルダにPython環境を構築します", "実行しますか？", $choice, 1)
if ($answer -ne 0) {
	exit
}

echo "### python-3.12.3-embed-win32をダウンロードしてインストールします"

$install_dir = ".\python32"
New-Item -ItemType Directory -Force -Path $install_dir
cd $install_dir
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.SecurityProtocolType]::Tls12;
wget "https://www.python.org/ftp/python/3.12.3/python-3.12.3-embed-win32.zip" -O python.zip
Expand-Archive -Path python.zip -DestinationPath ./ -Force
rm python.zip

$pth = Convert-Path ".\python312._pth"
echo "### $pth を編集します"

(Get-Content $pth) | foreach { $_ -replace "#import site", "import site" } | Set-Content $pth

echo "### pipをダウンロードしてインストールします"

[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.SecurityProtocolType]::Tls12;
wget "https://bootstrap.pypa.io/get-pip.py" -O .\get-pip.py
.\python .\get-pip.py --no-warn-script-location
rm .\get-pip.py

echo "### pywin32をインストールします"

.\python -m pip install pywin32

echo "### Python環境の構築が完了しました"

Pause
