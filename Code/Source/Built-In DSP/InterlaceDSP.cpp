#include "StdAfx.h"

#include "InterlaceDSP.h"
#include <AlternativeAudio\AAAttributeTypes.h>

namespace AlternativeAudio {
    namespace DSP {
		void InterleaveDSPEffect::Process(AudioFrame::Type format, float * buffer, long long len, AAAttributeHandler * attr) {
			if (attr->hasAttr(Attributes::Source::Deinterlaced)) {
				//AZ::AttributeData<bool>* deinterlaced = (AZ::AttributeData<bool>*)attr->getAttr(Attributes::Source::Deinterlaced);
				//if (!deinterlaced->Get(nullptr)) return; //we are not deinterlaced
				if (!attr->getAttrValue<bool>(Attributes::Source::Deinterlaced)) return;
			} else {
				return; //we dont have a deinterlaced attribute
			}

#			define SET_BUFFER(type) \
				AudioFrame::Deinterlaced::##type##* in = new AudioFrame::Deinterlaced::##type##[len]; \
				memcpy(in, buffer, len * sizeof(AudioFrame::##type##) / 4); \
				AudioFrame::##type##* out = (AudioFrame::##type##*)buffer;

#			define COPY_CHANNEL(channel) out[i].##channel = in->##channel##[i];
			switch (format) {
			break;
			case AudioFrame::Type::eT_af2:
			{
				SET_BUFFER(af2);

				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, false);
			}
			break;
			case AudioFrame::Type::eT_af21:
			{
				SET_BUFFER(af21);

				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(sub);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, false);
			}
			break;
			case AudioFrame::Type::eT_af3:
			{
				SET_BUFFER(af3);

				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(center);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, false);
			}
			break;
			case AudioFrame::Type::eT_af31:
			{
				SET_BUFFER(af31);

				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(sub);
					COPY_CHANNEL(center);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, false);
			}
			break;
			case AudioFrame::Type::eT_af4:
			{
				SET_BUFFER(af4);

				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(bleft);
					COPY_CHANNEL(bright);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, false);
			}
			break;
			case AudioFrame::Type::eT_af41:
			{
				SET_BUFFER(af41);

				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(bleft);
					COPY_CHANNEL(bright);
					COPY_CHANNEL(sub);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, false);
			}
			break;
			case AudioFrame::Type::eT_af5:
			{
				SET_BUFFER(af5);

				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(center);
					COPY_CHANNEL(bleft);
					COPY_CHANNEL(bright);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, false);
			}
			break;
			case AudioFrame::Type::eT_af51:
			{
				SET_BUFFER(af51);

				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(center);
					COPY_CHANNEL(bleft);
					COPY_CHANNEL(bright);
					COPY_CHANNEL(sub);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, false);
			}
			break;
			case AudioFrame::Type::eT_af7:
			{
				SET_BUFFER(af7);

				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(center);
					COPY_CHANNEL(bleft);
					COPY_CHANNEL(bright);
					COPY_CHANNEL(sleft);
					COPY_CHANNEL(sright);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, false);
			}
			break;
			case AudioFrame::Type::eT_af71:
			{
				SET_BUFFER(af71);

				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(center);
					COPY_CHANNEL(bleft);
					COPY_CHANNEL(bright);
					COPY_CHANNEL(sleft);
					COPY_CHANNEL(sright);
					COPY_CHANNEL(sub);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, false);
			}
			break;
			}
#			undef SET_BUFFER
#			undef COPY_CHANNEL
		}
		//----------------------------------------------
		void DeinterleaveDSPEffect::Process(AudioFrame::Type format, float * buffer, long long len, AAAttributeHandler * attr) {
			if (attr->hasAttr(Attributes::Source::Deinterlaced)) {
				//AZ::AttributeData<bool>* deinterlaced = (AZ::AttributeData<bool>*)attr->getAttr(Attributes::Source::Deinterlaced);
				//if (deinterlaced->Get(nullptr)) return; //we are already deinterlaced
				if (attr->getAttrValue<bool>(Attributes::Source::Deinterlaced)) return;
			}

#			define SET_BUFFER(type) \
				AudioFrame::##type##* in = new AudioFrame::##type##[len]; \
				memcpy(in, buffer, len * sizeof(AudioFrame::##type##) / 4); \
				AudioFrame::Deinterlaced::##type##* out = (AudioFrame::Deinterlaced::##type##*)buffer;
#			define COPY_CHANNEL(channel) out->##channel##[i] = in[i].##channel##;
			switch (format) {
			break;
			case AudioFrame::Type::eT_af2:
			{
				SET_BUFFER(af2);
				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, true);
			}
			break;
			case AudioFrame::Type::eT_af21:
			{
				SET_BUFFER(af21);
				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(sub);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, true);
			}
			break;
			case AudioFrame::Type::eT_af3:
			{
				SET_BUFFER(af3);
				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(center);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, true);
			}
			break;
			case AudioFrame::Type::eT_af31:
			{
				SET_BUFFER(af31);
				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(center);
					COPY_CHANNEL(sub);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, true);
			}
			break;
			case AudioFrame::Type::eT_af4:
			{
				SET_BUFFER(af4);
				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(bleft);
					COPY_CHANNEL(bright);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, true);
			}
			break;
			case AudioFrame::Type::eT_af41:
			{
				SET_BUFFER(af41);
				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(bleft);
					COPY_CHANNEL(bright);
					COPY_CHANNEL(sub);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, true);
			}
			break;
			case AudioFrame::Type::eT_af5:
			{
				SET_BUFFER(af5);
				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(center);
					COPY_CHANNEL(bleft);
					COPY_CHANNEL(bright);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, true);
			}
			break;
			case AudioFrame::Type::eT_af51:
			{
				SET_BUFFER(af51);
				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(center);
					COPY_CHANNEL(bleft);
					COPY_CHANNEL(bright);
					COPY_CHANNEL(sub);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, true);
			}
			break;
			case AudioFrame::Type::eT_af7:
			{
				SET_BUFFER(af7);
				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(bleft);
					COPY_CHANNEL(bright);
					COPY_CHANNEL(center);
					COPY_CHANNEL(sleft);
					COPY_CHANNEL(sright);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, true);
			}
			break;
			case AudioFrame::Type::eT_af71:
			{
				SET_BUFFER(af71);
				for (long long i = 0; i < len; i++) {
					COPY_CHANNEL(left);
					COPY_CHANNEL(right);
					COPY_CHANNEL(bleft);
					COPY_CHANNEL(bright);
					COPY_CHANNEL(center);
					COPY_CHANNEL(sleft);
					COPY_CHANNEL(sright);
					COPY_CHANNEL(sub);
				}

				delete in;
				attr->setAttr(Attributes::Source::Deinterlaced, true);
			}
			break;
			}
#			undef SET_BUFFER
#			undef COPY_CHANNEL
		}
    }
}