echo "This script will fail to work if the xcode app is not installed"
sudo xcodebuild -license
sudo xcode-select --install
sudo xcode-select --switch /Applications/Xcode.app/Contents/Developer/
sudo xcodebuild -runFirstLaunch
xcodebuild -downloadComponent MetalToolchain
