#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "AlternativeAudioSystemComponent.h"
#include "AlternativeAudio\AudioSourceAsset.h"

#include "Built-in DSP\VolumeDSP.h"

#include <AzCore\RTTI\RTTI.h>

namespace AlternativeAudio {
	AlternativeAudioSystemComponent::AlternativeAudioSystemComponent() {
		this->defaultConvert = [](AudioFrame::Frame* from, AudioFrame::Frame* to, AudioFrame::Type inType, AudioFrame::Type outType, long long len) -> void {
			if (inType == AudioFrame::Type::eT_af1) { //Mono to...
				AudioFrame::af1 *in = (AudioFrame::af1*)(from);
				if (outType == AudioFrame::Type::eT_af1) { //Mono
					AudioFrame::af1 * ret = (AudioFrame::af1*)(to);
					for (int i = 0; i<len; i++) ret[i].mono = in[i].mono;
				} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
					AudioFrame::af2* ret = (AudioFrame::af2*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].mono;
						ret[i].right = in[i].mono;
					}
				} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
					AudioFrame::af21* ret = (AudioFrame::af21*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].mono;
						ret[i].right = in[i].mono;
						ret[i].sub = in[i].mono;
					}
				} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
					AudioFrame::af3* ret = (AudioFrame::af3*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].mono;
						ret[i].right = in[i].mono;
						ret[i].center = in[i].mono;
					}
				} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
					AudioFrame::af31* ret = (AudioFrame::af31*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].mono;
						ret[i].right = in[i].mono;
						ret[i].center = in[i].mono;
						ret[i].sub = in[i].mono;
					}
				} else if (outType == AudioFrame::Type::eT_af4) { //4 channels
					AudioFrame::af4* ret = (AudioFrame::af4*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].mono;
						ret[i].right = in[i].mono;
						ret[i].bleft = in[i].mono / 2;
						ret[i].bright = in[i].mono / 2;
					}
				} else if (outType == AudioFrame::Type::eT_af41) { //4.1 channels
					AudioFrame::af41* ret = (AudioFrame::af41*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].mono;
						ret[i].right = in[i].mono;
						ret[i].bleft = in[i].mono / 2;
						ret[i].bright = in[i].mono / 2;
						ret[i].sub = in[i].mono;
					}
				} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
					AudioFrame::af5* ret = (AudioFrame::af5*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].mono;
						ret[i].right = in[i].mono;
						ret[i].center = in[i].mono;
						ret[i].back.left = in[i].mono / 2;
						ret[i].back.right = in[i].mono / 2;
					}
				} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
					AudioFrame::af51* ret = (AudioFrame::af51*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].mono;
						ret[i].right = in[i].mono;
						ret[i].center = in[i].mono;
						ret[i].back.left = in[i].mono / 2;
						ret[i].back.right = in[i].mono / 2;
						ret[i].sub = in[i].mono;
					}
				} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
					AudioFrame::af7* ret = (AudioFrame::af7*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].mono;
						ret[i].right = in[i].mono;
						ret[i].center = in[i].mono;
						ret[i].side.left = in[i].mono / 2;
						ret[i].side.right = in[i].mono / 2;
						ret[i].back.left = in[i].mono / 3;
						ret[i].back.right = in[i].mono / 3;
					}
				} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
					AudioFrame::af71* ret = (AudioFrame::af71*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].mono;
						ret[i].right = in[i].mono;
						ret[i].center = in[i].mono;
						ret[i].side.left = in[i].mono / 2;
						ret[i].side.right = in[i].mono / 2;
						ret[i].back.left = in[i].mono / 3;
						ret[i].back.right = in[i].mono / 3;
						ret[i].sub = in[i].mono;
					}
				}
			} else if (inType == AudioFrame::Type::eT_af2) { //2 channels to...
				AudioFrame::af2 *in = (AudioFrame::af2*)(from);
				if (outType == AudioFrame::Type::eT_af1) { //Mono
					AudioFrame::af1* ret = (AudioFrame::af1*)(to);
					for (int i = 0; i < len; i++)
						ret[i].mono = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
				} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
					AudioFrame::af2* ret = (AudioFrame::af2*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
					}
				} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
					AudioFrame::af21* ret = (AudioFrame::af21*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
					AudioFrame::af3* ret = (AudioFrame::af3*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
					AudioFrame::af31* ret = (AudioFrame::af31*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af4) { //4 channels
					AudioFrame::af4* ret = (AudioFrame::af4*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].back.left = in[i].left / 2;
						ret[i].back.right = in[i].right / 2;
					}
				} else if (outType == AudioFrame::Type::eT_af41) { //4.1 channels
					AudioFrame::af41* ret = (AudioFrame::af41*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].back.left = in[i].left / 2;
						ret[i].back.right = in[i].right / 2;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
					AudioFrame::af5* ret = (AudioFrame::af5*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
						ret[i].back.left = in[i].left / 2;
						ret[i].back.right = in[i].right / 2;
					}
				} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
					AudioFrame::af51* ret = (AudioFrame::af51*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
						ret[i].back.left = in[i].left / 2;
						ret[i].back.right = in[i].right / 2;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
					AudioFrame::af7* ret = (AudioFrame::af7*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
						ret[i].side.left = in[i].left / 2;
						ret[i].side.right = in[i].right / 2;
						ret[i].back.left = in[i].left / 3;
						ret[i].back.right = in[i].right / 3;
					}
				} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
					AudioFrame::af71* ret = (AudioFrame::af71*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
						ret[i].side.left = in[i].left / 2;
						ret[i].side.right = in[i].right / 2;
						ret[i].back.left = in[i].left / 3;
						ret[i].back.right = in[i].right / 3;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					}
				}
			} else if (inType == AudioFrame::Type::eT_af21) { //2.1 channels to...
				AudioFrame::af21 *in = (AudioFrame::af21*)(from);
				if (outType == AudioFrame::Type::eT_af1) { //Mono
					AudioFrame::af1* ret = (AudioFrame::af1*)(to);
					for (int i = 0; i < len; i++)
						ret[i].mono = AZ::GetClamp(in[i].left + in[i].right + in[i].sub, -1.0f, 1.0f);
				} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
					AudioFrame::af2* ret = (AudioFrame::af2*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
					AudioFrame::af21* ret = (AudioFrame::af21*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
					AudioFrame::af3* ret = (AudioFrame::af3*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].right + in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
					AudioFrame::af31* ret = (AudioFrame::af31*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].right + in[i].left, -1.0f, 1.0f);
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af4) { //4 channels
					AudioFrame::af4* ret = (AudioFrame::af4*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp((in[i].left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp((in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af41) { //4.1 channels
					AudioFrame::af41* ret = (AudioFrame::af41*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].back.left = in[i].left / 2;
						ret[i].back.right = in[i].right / 2;
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
					AudioFrame::af5* ret = (AudioFrame::af5*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].right + in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp((in[i].left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp((in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
					AudioFrame::af51* ret = (AudioFrame::af51*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
						ret[i].back.left = in[i].left / 2;
						ret[i].back.right = in[i].right / 2;
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
					AudioFrame::af7* ret = (AudioFrame::af7*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].left + in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].side.left = AZ::GetClamp((in[i].left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].side.right = AZ::GetClamp((in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp((in[i].left / 3) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp((in[i].right / 3) + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
					AudioFrame::af71* ret = (AudioFrame::af71*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
						ret[i].side.left = in[i].left / 2;
						ret[i].side.right = in[i].right / 2;
						ret[i].back.left = in[i].left / 3;
						ret[i].back.right = in[i].right / 3;
						ret[i].sub = in[i].sub;
					}
				}
			} else if (inType == AudioFrame::Type::eT_af3) { //3 channels to...
				AudioFrame::af3 *in = (AudioFrame::af3*)(from);
				if (outType == AudioFrame::Type::eT_af1) { //Mono
					AudioFrame::af1* ret = (AudioFrame::af1*)(to);
					for (int i = 0; i < len; i++)
						ret[i].mono = AZ::GetClamp(in[i].left + in[i].right + in[i].center, -1.0f, 1.0f);
				} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
					AudioFrame::af2* ret = (AudioFrame::af2*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
					AudioFrame::af21* ret = (AudioFrame::af21*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2), -1.0f, 1.0f);
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
					AudioFrame::af3* ret = (AudioFrame::af3*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
					}
				} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
					AudioFrame::af31* ret = (AudioFrame::af31*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af4) { //4 channels
					AudioFrame::af4* ret = (AudioFrame::af4*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2), -1.0f, 1.0f);
						ret[i].back.left = in[i].left / 2;
						ret[i].back.right = in[i].right / 2;
					}
				} else if (outType == AudioFrame::Type::eT_af41) { //4.1 channels
					AudioFrame::af41* ret = (AudioFrame::af41*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2), -1.0f, 1.0f);
						ret[i].back.left = in[i].left / 2;
						ret[i].back.right = in[i].right / 2;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
					AudioFrame::af5* ret = (AudioFrame::af5*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].back.left = in[i].left / 2;
						ret[i].back.right = in[i].right / 2;
					}
				} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
					AudioFrame::af51* ret = (AudioFrame::af51*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].back.left = in[i].left / 2;
						ret[i].back.right = in[i].right / 2;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
					AudioFrame::af7* ret = (AudioFrame::af7*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].side.left = in[i].left / 2;
						ret[i].side.right = in[i].right / 2;
						ret[i].back.left = in[i].left / 3;
						ret[i].back.right = in[i].right / 3;
					}
				} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
					AudioFrame::af71* ret = (AudioFrame::af71*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].side.left = in[i].left / 2;
						ret[i].side.right = in[i].right / 2;
						ret[i].back.left = in[i].left / 3;
						ret[i].back.right = in[i].right / 3;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center, -1.0f, 1.0f);
					}
				}
			} else if (inType == AudioFrame::Type::eT_af31) { //3.1 channels to...
				AudioFrame::af31 *in = (AudioFrame::af31*)(from);
				if (outType == AudioFrame::Type::eT_af1) { //Mono
					AudioFrame::af1* ret = (AudioFrame::af1*)(to);
					for (int i = 0; i < len; i++)
						ret[i].mono = AZ::GetClamp(in[i].left + in[i].right + in[i].center + in[i].sub, -1.0f, 1.0f);
				} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
					AudioFrame::af2* ret = (AudioFrame::af2*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
					AudioFrame::af21* ret = (AudioFrame::af21*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
					AudioFrame::af3* ret = (AudioFrame::af3*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
					AudioFrame::af31* ret = (AudioFrame::af31*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af4) { //4 channels
					AudioFrame::af4* ret = (AudioFrame::af4*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp((in[i].left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp((in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af41) { //4.1 channels
					AudioFrame::af41* ret = (AudioFrame::af41*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2), -1.0f, 1.0f);
						ret[i].back.left = in[i].left / 2;
						ret[i].back.right = in[i].right / 2;
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
					AudioFrame::af5* ret = (AudioFrame::af5*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp((in[i].left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp((in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
					AudioFrame::af51* ret = (AudioFrame::af51*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].back.left = in[i].left / 2;
						ret[i].back.right = in[i].right / 2;
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
					AudioFrame::af7* ret = (AudioFrame::af7*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].side.left = AZ::GetClamp((in[i].left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].side.right = AZ::GetClamp((in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp((in[i].left / 3) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp((in[i].right / 3) + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
					AudioFrame::af71* ret = (AudioFrame::af71*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].side.left = in[i].left / 2;
						ret[i].side.right = in[i].right / 2;
						ret[i].back.left = in[i].left / 3;
						ret[i].back.right = in[i].right / 3;
						ret[i].sub = in[i].sub;
					}
				}
			} else if (inType == AudioFrame::Type::eT_af4) { //4 channels to...
				AudioFrame::af4 *in = (AudioFrame::af4*)(from);
				if (outType == AudioFrame::Type::eT_af1) { //Mono
					AudioFrame::af1* ret = (AudioFrame::af1*)(to);
					for (int i = 0; i < len; i++)
						ret[i].mono = AZ::GetClamp(in[i].left + in[i].right + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
				} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
					AudioFrame::af2* ret = (AudioFrame::af2*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].back.right / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
					AudioFrame::af21* ret = (AudioFrame::af21*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + (in[i].back.left / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
					AudioFrame::af3* ret = (AudioFrame::af3*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].back.right / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
					AudioFrame::af31* ret = (AudioFrame::af31*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + (in[i].back.left / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af4) { //4 channels
					AudioFrame::af4* ret = (AudioFrame::af4*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
					}
				} else if (outType == AudioFrame::Type::eT_af41) { //4.1 channels
					AudioFrame::af41* ret = (AudioFrame::af41*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
					AudioFrame::af5* ret = (AudioFrame::af5*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].left / 2 + in[i].right / 2, -1.0f, 1.0f);
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
					}
				} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
					AudioFrame::af51* ret = (AudioFrame::af51*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].left / 2 + in[i].right / 2, -1.0f, 1.0f);
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
					AudioFrame::af7* ret = (AudioFrame::af7*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].left / 2 + in[i].right / 2, -1.0f, 1.0f);
						ret[i].side.left = AZ::GetClamp((in[i].left / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].side.right = AZ::GetClamp((in[i].right / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
					}
				} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
					AudioFrame::af71* ret = (AudioFrame::af71*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp(in[i].left / 2 + in[i].right / 2, -1.0f, 1.0f);
						ret[i].side.left = AZ::GetClamp((in[i].left / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].side.right = AZ::GetClamp((in[i].right / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
					}
				}
			} else if (inType == AudioFrame::Type::eT_af41) { //4.1 channels to...
				AudioFrame::af41 *in = (AudioFrame::af41*)(from);
				if (outType == AudioFrame::Type::eT_af1) { //Mono
					AudioFrame::af1* ret = (AudioFrame::af1*)(to);
					for (int i = 0; i < len; i++)
						ret[i].mono = AZ::GetClamp(in[i].left + in[i].right + in[i].back.left + in[i].back.right + in[i].sub, -1.0f, 1.0f);
				} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
					AudioFrame::af2* ret = (AudioFrame::af2*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].back.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right +  (in[i].back.right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
					AudioFrame::af21* ret = (AudioFrame::af21*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
					AudioFrame::af3* ret = (AudioFrame::af3*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].back.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].back.right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp((in[i].left / 2) + (in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
					AudioFrame::af31* ret = (AudioFrame::af31*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp((in[i].left / 2) + (in[i].right / 2), -1.0f, 1.0f);
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af4) { //4 channels
					AudioFrame::af4* ret = (AudioFrame::af4*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af41) { //4.1 channels
					AudioFrame::af41* ret = (AudioFrame::af41*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
					AudioFrame::af5* ret = (AudioFrame::af5*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp((in[i].left / 2) + (in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
					AudioFrame::af51* ret = (AudioFrame::af51*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp((in[i].left / 2) + (in[i].right / 2), -1.0f, 1.0f);
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
					AudioFrame::af7* ret = (AudioFrame::af7*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp((in[i].left / 2) + (in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].side.left = AZ::GetClamp((in[i].left / 2) + (in[i].back.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].side.right = AZ::GetClamp((in[i].right / 2) + (in[i].back.right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
					AudioFrame::af71* ret = (AudioFrame::af71*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = AZ::GetClamp((in[i].left / 2) + (in[i].right / 2), -1.0f, 1.0f);
						ret[i].side.left = AZ::GetClamp((in[i].left / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].side.right = AZ::GetClamp((in[i].right / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
						ret[i].sub = in[i].sub;
					}
				}
			} else if (inType == AudioFrame::Type::eT_af5) { //5 channels to...
				AudioFrame::af5 *in = (AudioFrame::af5*)(from);
				if (outType == AudioFrame::Type::eT_af1) { //Mono
					AudioFrame::af1* ret = (AudioFrame::af1*)(to);
					for (int i = 0; i < len; i++)
						ret[i].mono = AZ::GetClamp(in[i].left + in[i].right + in[i].center + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
				} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
					AudioFrame::af2* ret = (AudioFrame::af2*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
					AudioFrame::af21* ret = (AudioFrame::af21*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center + (in[i].back.left / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
					AudioFrame::af3* ret = (AudioFrame::af3*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].center = in[i].center;
					}
				} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
					AudioFrame::af31* ret = (AudioFrame::af31*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].center = in[i].center;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center + (in[i].back.left / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af4) { //4 channels
					AudioFrame::af4* ret = (AudioFrame::af4*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + in[i].center / 2, -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + in[i].center / 2, -1.0f, 1.0f);
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
					}
				} else if (outType == AudioFrame::Type::eT_af41) { //4.1 channels
					AudioFrame::af41* ret = (AudioFrame::af41*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2), -1.0f, 1.0f);
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].back.left + in[i].back.right + in[i].center, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
					AudioFrame::af5* ret = (AudioFrame::af5*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
					}
				} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
					AudioFrame::af51* ret = (AudioFrame::af51*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
					AudioFrame::af7* ret = (AudioFrame::af7*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].side.left = AZ::GetClamp((in[i].left / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].side.right = AZ::GetClamp((in[i].right / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
					}
				} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
					AudioFrame::af71* ret = (AudioFrame::af71*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].side.left = AZ::GetClamp((in[i].left / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].side.right = AZ::GetClamp((in[i].right / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
					}
				}
			} else if (inType == AudioFrame::Type::eT_af51) { //5.1 channels to...
				AudioFrame::af51 *in = (AudioFrame::af51*)(from);
				if (outType == AudioFrame::Type::eT_af1) { //Mono
					AudioFrame::af1* ret = (AudioFrame::af1*)(to);
					for (int i = 0; i < len; i++)
						ret[i].mono = AZ::GetClamp(in[i].left + in[i].right + in[i].center + in[i].back.left + in[i].back.right + in[i].sub, -1.0f, 1.0f);
				} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
					AudioFrame::af2* ret = (AudioFrame::af2*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2) + (in[i].back.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2) + (in[i].back.right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
					AudioFrame::af21* ret = (AudioFrame::af21*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
					AudioFrame::af3* ret = (AudioFrame::af3*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].back.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].back.right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f) / 2;
					}
				} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
					AudioFrame::af31* ret = (AudioFrame::af31*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].center = in[i].center;
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af4) { //4 channels
					AudioFrame::af4* ret = (AudioFrame::af4*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + in[i].center / 2 + in[i].sub, -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + in[i].center / 2 + in[i].sub, -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + in[i].sub, -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + in[i].sub, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af41) { //4.1 channels
					AudioFrame::af41* ret = (AudioFrame::af41*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + in[i].center / 2, -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + in[i].center / 2, -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left, -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right, -1.0f, 1.0f);
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
					AudioFrame::af5* ret = (AudioFrame::af5*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
					AudioFrame::af51* ret = (AudioFrame::af51*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
					AudioFrame::af7* ret = (AudioFrame::af7*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].side.left = AZ::GetClamp((in[i].left / 2) + (in[i].back.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].side.right = AZ::GetClamp((in[i].right / 2) + (in[i].back.right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
					AudioFrame::af71* ret = (AudioFrame::af71*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].side.left = AZ::GetClamp((in[i].left / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
						ret[i].side.right = AZ::GetClamp((in[i].right / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
						ret[i].sub = in[i].sub;
					}
				}
			} else if (inType == AudioFrame::Type::eT_af7) { //7 channels to...
				AudioFrame::af7 *in = (AudioFrame::af7*)(from);
				if (outType == AudioFrame::Type::eT_af1) { //Mono
					AudioFrame::af1* ret = (AudioFrame::af1*)(to);
					for (int i = 0; i < len; i++)
						ret[i].mono = AZ::GetClamp(in[i].left + in[i].right + in[i].center + in[i].side.left + in[i].side.right + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
				} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
					AudioFrame::af2* ret = (AudioFrame::af2*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2) + (in[i].side.left / 2) + (in[i].back.left / 3), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2) + (in[i].side.right / 2) + (in[i].back.right / 3), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
					AudioFrame::af21* ret = (AudioFrame::af21*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2) + (in[i].side.left / 2) + (in[i].back.left / 3), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2) + (in[i].side.right / 2) + (in[i].back.right / 3), -1.0f, 1.0f);
						ret[i].sub = (in[i].left + in[i].right + in[i].center + (in[i].side.left / 2) + (in[i].side.right / 2) + (in[i].back.left / 3) + (in[i].back.right / 3), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
					AudioFrame::af3* ret = (AudioFrame::af3*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].side.left / 2) + (in[i].back.left / 3), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].side.right / 2) + (in[i].back.right / 3), -1.0f, 1.0f);
						ret[i].center = in[i].center;
					}
				} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
					AudioFrame::af31* ret = (AudioFrame::af31*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].side.left / 2) + (in[i].back.left / 3), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].side.right / 2) + (in[i].back.right / 3), -1.0f, 1.0f);
						ret[i].center = in[i].center;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center + (in[i].side.left / 2) + (in[i].side.right / 2) + (in[i].back.left / 3) + (in[i].back.right / 3), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af4) { //4 channels
					AudioFrame::af4* ret = (AudioFrame::af4*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + in[i].center / 2, -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + in[i].center / 2, -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].side.left / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af41) { //4.1 channels
					AudioFrame::af41* ret = (AudioFrame::af41*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2) + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2) + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center + in[i].side.left + in[i].side.right + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
					AudioFrame::af5* ret = (AudioFrame::af5*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].side.right / 2), -1.0f, 1.0f);
						ret[i].center = in[i].center;
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].side.left / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
					AudioFrame::af51* ret = (AudioFrame::af51*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].side.right / 2), -1.0f, 1.0f);
						ret[i].center = in[i].center;
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].side.right / 2), -1.0f, 1.0f);
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center + (in[i].side.left / 2) + (in[i].side.right / 2) + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
					AudioFrame::af7* ret = (AudioFrame::af7*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].side.left = in[i].side.left;
						ret[i].side.right = in[i].side.right;
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
					}
				} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
					AudioFrame::af71* ret = (AudioFrame::af71*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].side.left = in[i].side.left;
						ret[i].side.right = in[i].side.right;
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
						ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center + in[i].side.left + in[i].side.right + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
					}
				}
			} else if (inType == AudioFrame::Type::eT_af71) { //7.1 channels to...
				AudioFrame::af71 *in = (AudioFrame::af71*)(from);
				if (outType == AudioFrame::Type::eT_af1) { //Mono
					AudioFrame::af1* ret = (AudioFrame::af1*)(to);
					for (int i = 0; i < len; i++)
						ret[i].mono = AZ::GetClamp(in[i].left + in[i].right + in[i].center + in[i].side.left + in[i].side.right + in[i].back.left + in[i].back.right + in[i].sub, -1.0f, 1.0f);
				} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
					AudioFrame::af2* ret = (AudioFrame::af2*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2) + (in[i].side.left / 2) + (in[i].back.left / 3) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2) + (in[i].side.right / 2) + (in[i].back.right / 3) + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
					AudioFrame::af21* ret = (AudioFrame::af21*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2) + (in[i].side.left / 2) + (in[i].back.left / 3), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2) + (in[i].side.right / 2) + (in[i].back.right / 3), -1.0f, 1.0f);
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
					AudioFrame::af3* ret = (AudioFrame::af3*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].side.left / 2) + (in[i].back.left / 3) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].side.right / 2) + (in[i].back.right / 3) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
					AudioFrame::af31* ret = (AudioFrame::af31*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].side.left / 2) + (in[i].back.left / 3), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].side.right / 2) + (in[i].back.right / 3), -1.0f, 1.0f);
						ret[i].center = in[i].center;
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af4) { //4 channels
					AudioFrame::af4* ret = (AudioFrame::af4*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + in[i].center / 2 + in[i].sub, -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + in[i].center / 2 + in[i].sub, -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].side.left / 2) + in[i].sub, -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].side.left / 2) + in[i].sub, -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af41) { //4.1 channels
					AudioFrame::af41* ret = (AudioFrame::af41*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2) + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2) + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
					AudioFrame::af5* ret = (AudioFrame::af5*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].side.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].side.right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].side.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].side.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
					AudioFrame::af51* ret = (AudioFrame::af51*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].side.right / 2), -1.0f, 1.0f);
						ret[i].center = in[i].center;
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].side.left / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].side.right / 2), -1.0f, 1.0f);
						ret[i].sub = in[i].sub;
					}
				} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
					AudioFrame::af7* ret = (AudioFrame::af7*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].side.left = AZ::GetClamp(in[i].side.left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].side.right = AZ::GetClamp(in[i].side.right + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].sub / 2), -1.0f, 1.0f);
						ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].sub / 2), -1.0f, 1.0f);
					}
				} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
					AudioFrame::af71* ret = (AudioFrame::af71*)(to);
					for (int i = 0; i < len; i++) {
						ret[i].left = in[i].left;
						ret[i].right = in[i].right;
						ret[i].center = in[i].center;
						ret[i].side.left = in[i].side.left;
						ret[i].side.right = in[i].side.right;
						ret[i].back.left = in[i].back.left;
						ret[i].back.right = in[i].back.right;
						ret[i].sub = in[i].sub;
					}
				}
			}
		};
		this->defaultMix = [](AudioFrame::Frame* output, AudioFrame::Frame* source, AudioFrame::Type frameType, long long len) -> void {
		#define SET_BUFFERS(type) \
			AlternativeAudio::AudioFrame::##type##* out = (AlternativeAudio::AudioFrame::##type##*)output; \
			AlternativeAudio::AudioFrame::##type##* src = (AlternativeAudio::AudioFrame::##type##*)source;

		#define SET_CHANNEL(channel) \
			if (out[i].##channel == 0.0f) out[i].##channel = src[i].##channel; \
			else out[i].##channel += src[i].##channel; \
			out[i].##channel = AZ::GetClamp(out[i].##channel, -1.0f, 1.0f);

			if (frameType == AlternativeAudio::AudioFrame::Type::eT_af1) {
				SET_BUFFERS(af1);
				for (int i = 0; i < len; i++) {
					SET_CHANNEL(mono);
				}
			} else if (frameType == AlternativeAudio::AudioFrame::Type::eT_af2) {
				SET_BUFFERS(af2);
				for (int i = 0; i < len; i++) {
					SET_CHANNEL(left);
					SET_CHANNEL(right);
				}
			} else if (frameType == AlternativeAudio::AudioFrame::Type::eT_af21) {
				SET_BUFFERS(af21);
				for (int i = 0; i < len; i++) {
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(sub);
				}
			} else if (frameType == AlternativeAudio::AudioFrame::Type::eT_af3) {
				SET_BUFFERS(af3);
				for (int i = 0; i < len; i++) {
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(center);
				}
			} else if (frameType == AlternativeAudio::AudioFrame::Type::eT_af31) {
				SET_BUFFERS(af31);
				for (int i = 0; i < len; i++) {
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(center);
					SET_CHANNEL(sub);
				}
			} else if (frameType == AlternativeAudio::AudioFrame::Type::eT_af4) {
				SET_BUFFERS(af4);
				for (int i = 0; i < len; i++) {
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(bleft);
					SET_CHANNEL(bright);
				}
			} else if (frameType == AlternativeAudio::AudioFrame::Type::eT_af41) {
				SET_BUFFERS(af41);
				for (int i = 0; i < len; i++) {
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(bleft);
					SET_CHANNEL(bright);
					SET_CHANNEL(sub);
				}
			} else if (frameType == AlternativeAudio::AudioFrame::Type::eT_af5) {
				SET_BUFFERS(af5);
				for (int i = 0; i < len; i++) {
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(center);
					SET_CHANNEL(bleft);
					SET_CHANNEL(bright);
				}
			} else if (frameType == AlternativeAudio::AudioFrame::Type::eT_af51) {
				SET_BUFFERS(af51);
				for (int i = 0; i < len; i++) {
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(center);
					SET_CHANNEL(bleft);
					SET_CHANNEL(bright);
					SET_CHANNEL(sub);
				}
			} else if (frameType == AlternativeAudio::AudioFrame::Type::eT_af7) {
				SET_BUFFERS(af7);
				for (int i = 0; i < len; i++) {
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(center);
					SET_CHANNEL(sleft);
					SET_CHANNEL(sright);
					SET_CHANNEL(bleft);
					SET_CHANNEL(bright);
				}
			} else if (frameType == AlternativeAudio::AudioFrame::Type::eT_af71) {
				SET_BUFFERS(af71);
				for (int i = 0; i < len; i++) {
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(center);
					SET_CHANNEL(sleft);
					SET_CHANNEL(sright);
					SET_CHANNEL(bleft);
					SET_CHANNEL(bright);
					SET_CHANNEL(sub);
				}
			}
		#undef SET_BUFFERS
		#undef SET_CHANNEL
		};

		this->currentConvert = defaultConvert;
		this->currentMix = defaultMix;

		this->m_MasterDevice = nullptr;
		this->m_NullDeviceInfo = AZStd::vector<OAudioDeviceInfo>(0);
		this->m_NullDeviceInfo.shrink_to_fit();
	}

	AlternativeAudioSystemComponent::~AlternativeAudioSystemComponent() {
		this->m_sourceLibFuncs->clear();
		delete this->m_sourceLibFuncs;

		this->m_sourceLibNames->clear();
		delete this->m_sourceLibNames;

		this->m_dspLibFuncs->clear();
		delete this->m_dspLibFuncs;

		this->m_dspLibNames->clear();
		delete this->m_dspLibNames;

		this->m_deviceProviders->clear();
		delete this->m_deviceProviders;

		this->m_deviceLibNames->clear();
		delete this->m_deviceLibNames;

		//for (int i = 0; i < eDS_Count; i++) {
		//	for (std::pair<unsigned long long, AADSPEffect *> effect : this->m_dspEffects[i])
		//		//delete effect.second;
		//		effect.second->Release();
		//	this->m_dspEffects[i].clear();
		//}
		//delete[] this->m_dspEffects;

		//delete this->interlaceDSP;
		//delete this->deinterlaceDSP;
		this->interlaceDSP->Release();
		this->deinterlaceDSP->Release();
	}

	/*
	public:
		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
		}
	*/

	void AlternativeAudioSystemComponent::Reflect(AZ::ReflectContext* context) {
		if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context)) {
			//reflection
			serialize->Class<AlternativeAudioSystemComponent, AZ::Component>()
				->Version(0)
				->SerializerForEmptyClass();

			//class reflection
			AASmartRef::Reflect(serialize);
			AAError::Reflect(serialize);
			AAErrorHandler::Reflect(serialize);
			AAFlagHandler::Reflect(serialize);
			AudioSourceTime::Reflect(serialize);
			AADSPEffect::Reflect(serialize);
			AADSPEffectHandler::Reflect(serialize);
			AADSPDeviceEffectHandler::Reflect(serialize);
			IAudioSource::Reflect(serialize);
			DSP::VolumeDSPEffect::Reflect(serialize);
			DSP::InterleaveDSPEffect::Reflect(serialize);
			DSP::DeinterleaveDSPEffect::Reflect(serialize);
			OAudioDeviceInfo::Reflect(serialize);
			OAudioDevice::Reflect(serialize);
			OAudioDeviceProvider::Reflect(serialize);

			//edit context
			if (AZ::EditContext* ec = serialize->GetEditContext()) {
				ec->Class<AlternativeAudioSystemComponent>("AlternativeAudio", "Provides an alternative audio system for usage in lumberyard.")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Audio")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System", 0xc94d118b))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					;
			}
		}

		AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
		if (behaviorContext) {
			///constants and enums
			behaviorContext->Class<AADSPSection>("AADSPSection")
				->Enum<AADSPSection::eDS_PerSource_BC>("PerSource_BC")
				->Enum<AADSPSection::eDS_PerSource_AC>("PerSource_AC")
				->Enum<AADSPSection::eDS_PerSource_ARS>("PerSource_ARS")
				->Enum<AADSPSection::eDS_Output>("Output");

			behaviorContext->Class<AADSPProcessType>("AADSPProcessType")
				->Enum<AADSPProcessType::eDPT_Buffer>("Buffer")
				->Enum<AADSPProcessType::eDPT_Frame>("Frame");
			
			#define RESERVED_BIT(id) ->Enum<AASourceFlags::eAF_Reserved##id>("eAF_Reserved##id") 
			behaviorContext->Class<AASourceFlags>("AASourceFlags")
				->Enum<AASourceFlags::eAF_Loop>("Loop")
				->Enum<AASourceFlags::eAF_LoopSection>("LoopSection")
				->Enum<AASourceFlags::eAF_PausedOnStart>("PausedOnStart")
				->Enum<AASourceFlags::eAF_Deinterlaced>("eAF_Deinterlaced")
				RESERVED_BIT(0)
				RESERVED_BIT(1)
				RESERVED_BIT(2)
				RESERVED_BIT(3)
				RESERVED_BIT(4)
				RESERVED_BIT(5)
				RESERVED_BIT(6)
				RESERVED_BIT(7)
				RESERVED_BIT(8)
				RESERVED_BIT(9)
				RESERVED_BIT(10)
				RESERVED_BIT(11);
			#undef RESERVED_BIT

			behaviorContext->Class<AAResampleQuality>("AAResampleQuality")
				->Enum<AAResampleQuality::eAARQ_Best>("Best")
				->Enum<AAResampleQuality::eAARQ_Medium>("Medium")
				->Enum<AAResampleQuality::eAARQ_Fastest>("Fastest")
				->Enum<AAResampleQuality::eAARQ_Zero_Order_Hold>("Zero_Order_Hold")
				->Enum<AAResampleQuality::eAARQ_Linear>("Linear")
				->Enum<AAResampleQuality::eAARQ_Misc0>("Misc0")
				->Enum<AAResampleQuality::eAARQ_Misc1>("Misc1")
				->Enum<AAResampleQuality::eAARQ_Misc2>("Misc2")
				->Enum<AAResampleQuality::eAARQ_Misc3>("Misc3")
				->Enum<AAResampleQuality::eAARQ_Misc4>("Misc4");

			///audio frame types
			behaviorContext->Class<AudioFrame::Type>("AAAudioFrame")
				->Enum<AudioFrame::Type::eT_af1>("af1")
				->Enum<AudioFrame::Type::eT_af2>("af2")
				->Enum<AudioFrame::Type::eT_af21>("af21")
				->Enum<AudioFrame::Type::eT_af3>("af3")
				->Enum<AudioFrame::Type::eT_af31>("af31")
				->Enum<AudioFrame::Type::eT_af4>("af4")
				->Enum<AudioFrame::Type::eT_af41>("af41")
				->Enum<AudioFrame::Type::eT_af5>("af5")
				->Enum<AudioFrame::Type::eT_af51>("af51")
				->Enum<AudioFrame::Type::eT_af7>("af7")
				->Enum<AudioFrame::Type::eT_af71>("af71");

			///Methods
			AudioSourceTime::Behavior(behaviorContext); //time
			AASmartRef::Behavior(behaviorContext); //smart reference

			//errors
			AAError::Behavior(behaviorContext);
			AAErrorHandler::Behavior(behaviorContext);

			AAFlagHandler::Behavior(behaviorContext); //flag
			IAudioSource::Behavior(behaviorContext); //audio source

			//dsp effect
			AADSPEffect::Behavior(behaviorContext);
			AADSPEffectHandler::Behavior(behaviorContext);
			AADSPDeviceEffectHandler::Behavior(behaviorContext);

			//output device
			OAudioDeviceInfo::Behavior(behaviorContext);
			OAudioDevice::Behavior(behaviorContext);
			OAudioDeviceProvider::Behavior(behaviorContext);

			///Buses
			//Alternative Audio EBus
			behaviorContext->EBus<AlternativeAudioSourceBus>("AlternativeAudioSourceBus")
				//basic audio library system
				->Event("NewAudioSource", &AlternativeAudioSourceBus::Events::NewAudioSource)
				->Event("GetAudioLibraryNames", &AlternativeAudioSourceBus::Events::GetAudioLibraryNames);

			//DSP Ebus
			behaviorContext->EBus<AlternativeAudioDSPBus>("AlternativeAudioDSPBus")
				//basic dsp library system
				->Event("NewDSPEffect", &AlternativeAudioDSPBus::Events::NewDSPEffect)
				->Event("GetDSPEffectNames", &AlternativeAudioDSPBus::Events::GetDSPEffectNames)
				//basic dsp system
				->Event("AddEffect", &AlternativeAudioDSPBus::Events::AddEffect)
				->Event("AddEffectFreeSlot", &AlternativeAudioDSPBus::Events::AddEffectFreeSlot)
				->Event("GetEffect", &AlternativeAudioDSPBus::Events::GetEffect)
				->Event("RemoveEffect", &AlternativeAudioDSPBus::Events::RemoveEffect);

			//Audio Device bus
			behaviorContext->EBus<AlternativeAudioDeviceBus>("AlternativeAudioDeviceBus")
				->Event("NewDevice", &AlternativeAudioDeviceBus::Events::NewDevice)
				->Event("GetPlaybackLibraryNames", &AlternativeAudioDeviceBus::Events::GetPlaybackLibraryNames)
				->Event("GetPlaybackDevices", &AlternativeAudioDeviceBus::Events::GetPlaybackDevices)
				->Event("GetDefaultPlaybackDevice", &AlternativeAudioDeviceBus::Events::GetDefaultPlaybackDevice)
				->Event("GetDeviceProvider", &AlternativeAudioDeviceBus::Events::GetDeviceProvider)
				//----
				->Event("SetMasterDevice", &AlternativeAudioDeviceBus::Events::SetMasterDevice)
				->Event("SetStream", &AlternativeAudioDeviceBus::Events::SetStream)
				->Event("SetResampleQuality", &AlternativeAudioDeviceBus::Events::SetResampleQuality)
				->Event("GetDeviceInfo", &AlternativeAudioDeviceBus::Events::GetDeviceInfo)
				->Event("PlaySource", &AlternativeAudioDeviceBus::Events::PlaySource)
				->Event("PauseSource", &AlternativeAudioDeviceBus::Events::PauseSource)
				->Event("ResumeSource", &AlternativeAudioDeviceBus::Events::ResumeSource)
				->Event("StopSource", &AlternativeAudioDeviceBus::Events::StopSource)
				->Event("IsPlaying", &AlternativeAudioDeviceBus::Events::IsPlaying)
				->Event("GetTime", &AlternativeAudioDeviceBus::Events::GetTime)
				->Event("SetTime", &AlternativeAudioDeviceBus::Events::SetTime);

			//volume dsp Ebus
			behaviorContext->EBus<DSP::VolumeDSPBus>("AAVolumeDSPBus")
				->Event("SetVol", &DSP::VolumeDSPBus::Events::SetVol)
				->Event("GetVol", &DSP::VolumeDSPBus::Events::GetVol);
		}
	}

	void AlternativeAudioSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
		provided.push_back(AZ_CRC("AlternativeAudioService", 0x2eb4e627));
	}

	void AlternativeAudioSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible) {
		incompatible.push_back(AZ_CRC("AlternativeAudioService", 0x2eb4e627));
	}

	void AlternativeAudioSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required) {
		(void)required;
	}

	void AlternativeAudioSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent) {
		(void)dependent;
	}

	void AlternativeAudioSystemComponent::Init() {
		this->m_sourceLibFuncs = new AZStd::unordered_map<AZ::Crc32, NewAudioSourceFunc>();
		this->m_sourceLibNames = new AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>();

		this->m_dspLibFuncs = new AZStd::unordered_map<AZ::Crc32, NewDSPEffectFunc>();
		this->m_dspLibNames = new AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>();

		this->m_deviceProviders = new AZStd::unordered_map<AZ::Crc32, OAudioDeviceProvider*>();
		this->m_deviceLibNames = new AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>();
		
		////this->m_dspEffects = new AZStd::unordered_map<unsigned long long, AADSPEffect *>[eDS_Count];
		//this->m_dspEffects = new std::map<unsigned long long, AADSPEffect *>[eDS_Count];

		this->interlaceDSP = new DSP::InterleaveDSPEffect(nullptr);
		this->interlaceDSP->AddRef();
		this->deinterlaceDSP = new DSP::DeinterleaveDSPEffect(nullptr);
		this->deinterlaceDSP->AddRef();

		this->RegisterDSPEffect(
			"AAVolumeControl",
			AZ_CRC("AAVolumeControl", 0x722dd2a9),
			[](void* userdata)-> AADSPEffect* { return new DSP::VolumeDSPEffect(userdata); }
		);

		this->RegisterDSPEffect(
			"AAInterleave",
			AZ_CRC("AAInterleave", 0x0ac7fd53),
			[&](void* userdata)-> AADSPEffect* { return this->interlaceDSP; } //why create more than one interlace dsp effect?
		);

		this->RegisterDSPEffect(
			"AADeinterleave",
			AZ_CRC("AADeinterleave", 0x4a642bed),
			[&](void* userdata)-> AADSPEffect* { return this->deinterlaceDSP; } //why create more than one deinterlace dsp effect?
		);
	}

	void AlternativeAudioSystemComponent::Activate() {
		AlternativeAudioRequestBus::Handler::BusConnect();
		AlternativeAudioSourceBus::Handler::BusConnect();
		AlternativeAudioDSPBus::Handler::BusConnect();
		AlternativeAudioDeviceBus::Handler::BusConnect();
	}

	void AlternativeAudioSystemComponent::Deactivate() {
		AlternativeAudioRequestBus::Handler::BusDisconnect();
		AlternativeAudioSourceBus::Handler::BusDisconnect();
		AlternativeAudioDSPBus::Handler::BusDisconnect();
		AlternativeAudioDeviceBus::Handler::BusDisconnect();
	}

	////////////////////////////////////////////////////////////////////////
	// AlternativeAudioSourceBus interface implementation
	//--------------
	//IAudioSource
	void AlternativeAudioSystemComponent::RegisterAudioLibrary(AZStd::string libname, AZ::Crc32 crc, AZStd::vector<AZStd::string> filetypes, NewAudioSourceFunc ptr) {
		this->m_sourceLibFuncs->insert({ crc, ptr });
		this->m_sourceLibNames->push_back({ libname, crc });

		//build filetypes for audio asset
	}
	IAudioSource * AlternativeAudioSystemComponent::NewAudioSource(AZ::Crc32 crc, const char * path, void* userdata) {
		auto funcEntry = this->m_sourceLibFuncs->find(crc);
		if (funcEntry != this->m_sourceLibFuncs->end()) return funcEntry->second(path, userdata);
		return nullptr;
	}
	////////////////////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////////////////
	// AlternativeAudioDSPBus
	void AlternativeAudioSystemComponent::RegisterDSPEffect(AZStd::string libname, AZ::Crc32 crc, NewDSPEffectFunc ptr) {
		this->m_dspLibFuncs->insert({ crc, ptr });
		this->m_dspLibNames->push_back({ libname, crc });
	}
	AADSPEffect * AlternativeAudioSystemComponent::NewDSPEffect(AZ::Crc32 crc, void* userdata) {
		auto funcEntry = this->m_dspLibFuncs->find(crc);
		if (funcEntry != this->m_dspLibFuncs->end()) return funcEntry->second(userdata);
		return nullptr;
	}
	//--------------

	bool AlternativeAudioSystemComponent::AddEffect(AADSPSection section, AZ::Crc32 crc, void* userdata, unsigned long long slot) {
		AADSPEffect * effect = this->NewDSPEffect(crc, userdata);
		if (effect != nullptr) {
			if (AADSPDeviceEffectHandler::AddEffect(section, effect, slot)) return true;
			delete effect;
		}
		return false;
	}
	unsigned long long AlternativeAudioSystemComponent::AddEffectFreeSlot(AADSPSection section, AZ::Crc32 crc, void* userdata) {
		AADSPEffect * effect = this->NewDSPEffect(crc, userdata);
		if (effect != nullptr) {
			unsigned long long ret = AADSPDeviceEffectHandler::AddEffectFreeSlot(section, effect);
			if (ret >= 0) return ret;
			delete effect;
			return ret;
		}
		return -3;
	}
	////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	// AlternativeAudioDeviceBus
	void AlternativeAudioSystemComponent::RegisterPlaybackLibrary(AZStd::string libname, AZ::Crc32 crc, OAudioDeviceProvider* ptr) {
		this->m_deviceProviders->insert({ crc, ptr });
		this->m_deviceLibNames->push_back({ libname, crc });
	}
	OAudioDevice * AlternativeAudioSystemComponent::NewDevice(AZ::Crc32 crc, long long device, double samplerate, AlternativeAudio::AudioFrame::Type audioFormat, void* userdata) {
		auto providerEntry = this->m_deviceProviders->find(crc);
		if (providerEntry != this->m_deviceProviders->end()) return providerEntry->second->NewDevice(device, samplerate, audioFormat, userdata);
		return nullptr;
	}

	AZStd::vector<OAudioDeviceInfo>& AlternativeAudioSystemComponent::GetPlaybackDevices(AZ::Crc32 playbackLib) {
		auto providerEntry = this->m_deviceProviders->find(playbackLib);
		if (providerEntry != this->m_deviceProviders->end()) return providerEntry->second->GetDevices();
		return this->m_NullDeviceInfo;
	}

	long long AlternativeAudioSystemComponent::GetDefaultPlaybackDevice(AZ::Crc32 playbackLib) {
		auto providerEntry = this->m_deviceProviders->find(playbackLib);
		if (providerEntry != this->m_deviceProviders->end()) return providerEntry->second->GetDefaultDevice();
		return -1;
	}

	OAudioDeviceProvider* AlternativeAudioSystemComponent::GetDeviceProvider(AZ::Crc32 playbackLib) {
		auto providerEntry = this->m_deviceProviders->find(playbackLib);
		if (providerEntry != this->m_deviceProviders->end()) return providerEntry->second;
		return &(this->m_nullProvider);
	}
	//---------

	void AlternativeAudioSystemComponent::SetMasterDevice(OAudioDevice * device) {
		if (device) device->AddRef();
		if (this->m_MasterDevice) this->m_MasterDevice->Release();
		this->m_MasterDevice = device;
	}

	#define IF_DEVICE if (this->m_MasterDevice) this->m_MasterDevice
	#define IF_DEVICE_RET if (this->m_MasterDevice) return this->m_MasterDevice
	void AlternativeAudioSystemComponent::SetStream(double samplerate, AudioFrame::Type audioFormat, void * userdata) {
		IF_DEVICE->SetStream(samplerate, audioFormat, userdata);
	}
	void AlternativeAudioSystemComponent::SetResampleQuality(AAResampleQuality quality) {
		IF_DEVICE->SetResampleQuality(quality);
	}
	OAudioDeviceInfo AlternativeAudioSystemComponent::GetDeviceInfo() {
		IF_DEVICE_RET->GetDeviceInfo();
		return OAudioDeviceInfo();
	}
	long long AlternativeAudioSystemComponent::PlaySource(IAudioSource * source) {
		IF_DEVICE_RET->PlaySource(source);
		return -1;
	}
	void AlternativeAudioSystemComponent::PauseSource(long long id) {
		IF_DEVICE->PauseSource(id);
	}
	void AlternativeAudioSystemComponent::ResumeSource(long long id) {
		IF_DEVICE->ResumeSource(id);
	}
	void AlternativeAudioSystemComponent::StopSource(long long id) {
		IF_DEVICE->StopSource(id);
	}
	bool AlternativeAudioSystemComponent::IsPlaying(long long id) {
		IF_DEVICE_RET->IsPlaying(id);
		return false;
	}
	AudioSourceTime AlternativeAudioSystemComponent::GetTime(long long id) {
		IF_DEVICE_RET->GetTime(id);
		return AudioSourceTime();
	}
	void AlternativeAudioSystemComponent::SetTime(long long id, double time) {
		IF_DEVICE->SetTime(id, time);
	}
	#undef IF_DEVICE
	#undef IF_DEVICE_RET
	////////////////////////////////////////////////////////////////////////

	/*int GetSection(DSPSection section) {
		switch (section) {
		case eDS_PerSource_BC:
			return 0;
		case eDS_PerSource_AC:
			return 1;
		case eDS_PerSource_ARS:
			return 2;
		case eDS_Output:
			return 3;
		}
		return 0;
	}*/

	//--------------
	//basic DSP system
	//bool AlternativeAudioSystemComponent::AddDSPEffect(DSPSection section, AZ::Crc32 crc, void* userdata, unsigned long long slot) {
	//	int sectionInt = GetSection(section);
	//	if (this->m_dspEffects[sectionInt].count(slot) == 0) {
	//		AADSPEffect* effect = this->NewDSPEffect(crc, userdata);

	//		if (effect->GetDSPSection() & section) {
	//			this->m_dspEffects[sectionInt][slot] = effect;
	//			effect->AddRef();
	//			return true; //dsp effect is added to slot specified
	//		}

	//		delete effect;
	//		return false; //dsp effect is not suited for section specified
	//	}
	//	return false; //slot already has a dsp effect.
	//}
	//unsigned long long AlternativeAudioSystemComponent::AddDSPEffectFreeSlot(DSPSection section, AZ::Crc32 crc, void* userdata) {
	//	int sectionInt = GetSection(section);

	//	AADSPEffect* effect = this->NewDSPEffect(crc, userdata);
	//	effect->AddRef();
	//	if (!(effect->GetDSPSection() & section)) {
	//		//delete effect;
	//		effect->Release();
	//		return -1; //dsp is not for this specific dsp section
	//	}

	//	if (this->m_dspEffects[sectionInt].empty()) {
	//		this->m_dspEffects[sectionInt][0] = effect;
	//		//effect->AddRef();
	//		return 0;
	//	}

	//	//find an open slot
	//	auto end = this->m_dspEffects[sectionInt].rbegin();

	//	unsigned long long open = end->first+1; //store the next open end index

	//	for (auto it = this->m_dspEffects[sectionInt].begin(); it != --this->m_dspEffects[sectionInt].end(); it++) {
	//		auto it2 = it;
	//		it2++;

	//		if (it->first + 1 != it2->first) {
	//			//open slot
	//			open = it->first + 1;
	//			this->m_dspEffects[sectionInt][open] = effect;
	//			return open;
	//		}
	//	}

	//	this->m_dspEffects[sectionInt][open] = effect; //add effect to end
	//	return open;
	//}

	//AADSPEffect * AlternativeAudioSystemComponent::GetDSPEffect(DSPSection section, unsigned long long slot) {
	//	int sectionInt = GetSection(section);
	//	return this->m_dspEffects[sectionInt].at(slot);
	//}

	//bool AlternativeAudioSystemComponent::RemoveDSPEffect(DSPSection section, unsigned long long slot) {
	//	int sectionInt = GetSection(section);
	//	if (this->m_dspEffects[sectionInt].at(slot) != nullptr) {
	//		//delete this->m_dspEffects[section][slot];
	//		this->m_dspEffects[sectionInt][slot]->Release();
	//		this->m_dspEffects[sectionInt].erase(slot);
	//		return true; //dsp effect removed.
	//	}
	//	return false; //there is no dsp effect in slot specified
	//}
	//void AlternativeAudioSystemComponent::ProcessDSPEffects(DSPSection section, AudioFrame::Type format, float* buffer, long long len) {
	//	int sectionInt = GetSection(section);
	//	for (std::pair<unsigned long long, AADSPEffect *> effect : this->m_dspEffects[sectionInt]) {
	//		switch (effect.second->GetProcessType()) {
	//		case eDPT_Buffer:
	//			effect.second->Process(format, buffer, len);
	//			break;
	//		case eDPT_Frame:

	//			#define CASE_FORMAT(Format) \
	//				case AlternativeAudio::AudioFrame::Type::eT_##Format##: \
	//				{ \
	//					AlternativeAudio::AudioFrame::##Format##* buff = (AlternativeAudio::AudioFrame::##Format##*)buffer; \
	//					for (long long i = 0; i < len; i++) effect.second->ProcessFrame(format, (float*)&buff[i]); \
	//				}

	//			switch (format) {
	//				CASE_FORMAT(af1)
	//				CASE_FORMAT(af2)
	//				CASE_FORMAT(af21)
	//				CASE_FORMAT(af3)
	//				CASE_FORMAT(af31)
	//				CASE_FORMAT(af4)
	//				CASE_FORMAT(af41)
	//				CASE_FORMAT(af5)
	//				CASE_FORMAT(af51)
	//				CASE_FORMAT(af7)
	//				CASE_FORMAT(af71)
	//			}
	//			break;
	//			#undef CASE_FORMAT
	//		}
	//	}
	//}
	
	//--------------
	void AlternativeAudioSystemComponent::SetConvertFunction(ConvertAudioFrameFunc convertFunc) {
		if (!convertFunc) convertFunc = this->defaultConvert;
		this->currentConvert = convertFunc;
	}
	void AlternativeAudioSystemComponent::SetMixFunction(MixAudioFramesFunc mixFunc) {
		if (!mixFunc) mixFunc = this->defaultMix;
		this->currentMix = mixFunc;
	}

	//--------------
	int AlternativeAudioSystemComponent::GetNumberOfChannels(AudioFrame::Type type) {
		if (type == AudioFrame::Type::eT_af1) return 1;
		else if (type == AudioFrame::Type::eT_af2) return 2;
		else if (type == AudioFrame::Type::eT_af21) return 3;
		else if (type == AudioFrame::Type::eT_af3) return 3;
		else if (type == AudioFrame::Type::eT_af31) return 4;
		else if (type == AudioFrame::Type::eT_af4) return 4;
		else if (type == AudioFrame::Type::eT_af41) return 5;
		else if (type == AudioFrame::Type::eT_af5) return 5;
		else if (type == AudioFrame::Type::eT_af51) return 6;
		else if (type == AudioFrame::Type::eT_af7) return 7;
		else if (type == AudioFrame::Type::eT_af71) return 8;
		return 1;
	}

	AudioFrame::Type AlternativeAudioSystemComponent::GetAudioFormat(int numberOfChannels) {
		switch (numberOfChannels) {
		case 1:
			return AudioFrame::Type::eT_af1;
		case 2:
			return AudioFrame::Type::eT_af2;
		case 3:
			return AudioFrame::Type::eT_af21;
		case 4:
			return AudioFrame::Type::eT_af31;
		case 5:
			return AudioFrame::Type::eT_af5;
		case 6:
			return AudioFrame::Type::eT_af51;
		case 7:
			return AudioFrame::Type::eT_af7;
		case 8:
			return AudioFrame::Type::eT_af71;
		}

		return AudioFrame::Type::eT_af1;
	}
	////////////////////////////////////////////////////////////////////////
}
