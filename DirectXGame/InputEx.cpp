#include "input/Input.h"
bool Input::ReleseKey(BYTE keyNumber) const {
	if (!Input::key_[keyNumber] && Input::keyPre_[keyNumber]) {
		return true;
	}
	return false;
}