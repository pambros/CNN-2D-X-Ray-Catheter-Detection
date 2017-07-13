import subprocess
import platform

def CallCommand(_command, _getOutput=False, _verbose=True):
	if _verbose == True:
		print(_command)
	if _getOutput == True:
		ret = subprocess.Popen(_command, shell=True, stdout=subprocess.PIPE)
		# tmp = ret.stdout.read()
		out, err = ret.communicate()
		# print(out)
		# print(err)
		# print(ret)
		errcode = ret.returncode
		assert errcode == 0
		return out
		
	# ret = os.system(_command)
	ret = subprocess.call(_command, shell=True)
	print(ret)
	assert ret == 0
	return

# if you launch an executable, do not put .exe at the end of _command. It'll be added after following the os system.
def CallExecutable(_command, _args, _getOutput=False, _verbose=True):
	if platform.system() == 'Windows':
		# _command = SetWindowsPathNorm(_command)
		# dir = os.path.dirname(_command)
		# file = os.path.basename(_command)
		# dir = os.path.abspath(dir)
		# _command = '"' + dir + '\\' + file + ".exe" + '"'
		_command =  '"' + _command + ".exe" + '"'
		# _command = "start /REALTIME \"\" \"" + _command + "\""
		# _command = "start /HIGH \"\" \"" + _command + "\""
	_command = _command + " " + _args
	return CallCommand(_command, _getOutput, _verbose)