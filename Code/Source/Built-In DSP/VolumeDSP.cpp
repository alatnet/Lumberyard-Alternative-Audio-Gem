#include "StdAfx.h"

#include "VolumeDSP.h"

namespace AlternativeAudio{
    namespace DSP {
		VolumeDSPEffect::VolumeDSPEffect() {
			this->m_vol = 1.0f;
			VolumeDSPBus::Handler::BusConnect(this);
		}

		VolumeDSPEffect::VolumeDSPEffect(void* userdata) {
			this->m_vol = 1.0f;
			VolumeDSPBus::Handler::BusConnect(this);
		}

		VolumeDSPEffect::~VolumeDSPEffect() {
			VolumeDSPBus::Handler::BusDisconnect(this);
		}

		void VolumeDSPEffect::SetVol(float vol){
			if (vol >= 1.0f) vol = 1.0f;
			if (vol <= 0.0f) vol = 0.0f;
			//this->m_mutex.lock();
			this->m_vol = vol;
			//this->m_simd_vol = _mm_set1_ps(vol);
			//this->m_mutex.unlock();
		}

        void VolumeDSPEffect::Process(AudioFrame::Type format, float * buffer, long long len){
#		    define SET_BUFFERS(type) \
                AlternativeAudio::AudioFrame::##type##* out = (AlternativeAudio::AudioFrame::##type##*)buffer;
#			define SET_CHANNEL(channel) \
                out[i].##channel *= this->m_vol;
			//this->m_mutex.lock();
            switch(format){
            case AudioFrame::Type::eT_af1:
			{
				SET_BUFFERS(af1);
				for (long long i = 0; i < len; i++) {
					//this->ProcessFrame(format, (float*)&out[i]);
					SET_CHANNEL(mono);
				}
			}
                break;
            case AudioFrame::Type::eT_af2:
			{
				SET_BUFFERS(af2);
				for (long long i = 0; i < len; i++) {
					//this->ProcessFrame(format, (float*)&out[i]);
					SET_CHANNEL(left);
					SET_CHANNEL(right);
				}
			}
                break;
            case AudioFrame::Type::eT_af21:
			{
				SET_BUFFERS(af21);
				for (long long i = 0; i < len; i++) {
					//this->ProcessFrame(format, (float*)&out[i]);
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(sub);
				}
			}
                break;
            case AudioFrame::Type::eT_af3:
			{
				SET_BUFFERS(af3);
				for (long long i = 0; i < len; i++) {
					//this->ProcessFrame(format, (float*)&out[i]);
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(center);
				}
			}
                break;
            case AudioFrame::Type::eT_af31:
			{
				SET_BUFFERS(af31);
				for (long long i = 0; i < len; i++) {
					//this->ProcessFrame(format, (float*)&out[i]);
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(center);
					SET_CHANNEL(sub);
				}
			}
                break;
			case AudioFrame::Type::eT_af4:
			{
				SET_BUFFERS(af4);
				for (long long i = 0; i < len; i++) {
					//this->ProcessFrame(format, (float*)&out[i]);
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(bleft);
					SET_CHANNEL(bright);
				}
			}
				break;
			case AudioFrame::Type::eT_af41:
			{
				SET_BUFFERS(af41);
				for (long long i = 0; i < len; i++) {
					//this->ProcessFrame(format, (float*)&out[i]);
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(bleft);
					SET_CHANNEL(bright);
					SET_CHANNEL(sub);
				}
			}
				break;
            case AudioFrame::Type::eT_af5:
			{
				SET_BUFFERS(af5);
				for (long long i = 0; i < len; i++) {
					//this->ProcessFrame(format, (float*)&out[i]);
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(center);
					SET_CHANNEL(bleft);
					SET_CHANNEL(bright);
				}
			}
                break;
            case AudioFrame::Type::eT_af51:
			{
				SET_BUFFERS(af51);
				for (long long i = 0; i < len; i++) {
					//this->ProcessFrame(format, (float*)&out[i]);
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(center);
					SET_CHANNEL(bleft);
					SET_CHANNEL(bright);
					SET_CHANNEL(sub);
				}
			}
                break;
            case AudioFrame::Type::eT_af7:
			{
				SET_BUFFERS(af7);
				for (long long i = 0; i < len; i++) {
					//this->ProcessFrame(format, (float*)&out[i]);
					SET_CHANNEL(left);
					SET_CHANNEL(right);
					SET_CHANNEL(center);
					SET_CHANNEL(sleft);
					SET_CHANNEL(sright);
					SET_CHANNEL(bleft);
					SET_CHANNEL(bright);
				}
			}
                break;
            case AudioFrame::Type::eT_af71:
			{
				SET_BUFFERS(af71);
				for (long long i = 0; i < len; i++) {
					//this->ProcessFrame(format, (float*)&out[i]);
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
                break;
            }
			//this->m_mutex.unlock();
#			undef SET_BUFFERS
#			undef SET_CHANNEL
        }

		void VolumeDSPEffect::ProcessFrame(AudioFrame::Type format, float* frame){
#		    define SET_BUFFERS(type) \
                AlternativeAudio::AudioFrame::##type##* out = (AlternativeAudio::AudioFrame::##type##*)frame;
#			define SET_CHANNEL(channel) \
                out->##channel *= this->m_vol;
			//this->m_mutex.lock();
            switch(format){
            case AudioFrame::Type::eT_af1:
			{
				SET_BUFFERS(af1);
				SET_CHANNEL(mono);

				/*__m128 channels;
				channels.m128_f32[0] = out->mono;
				channels.m128_f32[1] = 0.0f;
				channels.m128_f32[2] = 0.0f;
				channels.m128_f32[3] = 0.0f;

				__m128 result = _mm_mul_ps(channels, m_simd_vol);
				out->mono = result.m128_f32[0];*/
			}
                break;
            case AudioFrame::Type::eT_af2:
			{
				SET_BUFFERS(af2);
				SET_CHANNEL(left);
				SET_CHANNEL(right);

				/*__m128 channels;
				channels.m128_f32[0] = out->left;
				channels.m128_f32[1] = out->right;
				channels.m128_f32[2] = 0.0f;
				channels.m128_f32[3] = 0.0f;

				__m128 result = _mm_mul_ps(channels, m_simd_vol);
				out->left = result.m128_f32[0];
				out->right = result.m128_f32[1];*/
			}
                break;
            case AudioFrame::Type::eT_af21:
			{
				SET_BUFFERS(af21);
				SET_CHANNEL(left);
				SET_CHANNEL(right);
				SET_CHANNEL(sub);

				/*__m128 channels;
				channels.m128_f32[0] = out->left;
				channels.m128_f32[1] = out->right;
				channels.m128_f32[2] = out->sub;
				channels.m128_f32[3] = 0.0f;

				__m128 result = _mm_mul_ps(channels, m_simd_vol);
				out->left = result.m128_f32[0];
				out->right = result.m128_f32[1];
				out->sub = result.m128_f32[2];*/
			}
                break;
            case AudioFrame::Type::eT_af3:
			{
				SET_BUFFERS(af3);
				SET_CHANNEL(left);
				SET_CHANNEL(right);
				SET_CHANNEL(center);

				/*__m128 channels;
				channels.m128_f32[0] = out->left;
				channels.m128_f32[1] = out->right;
				channels.m128_f32[2] = out->center;
				channels.m128_f32[3] = 0.0f;

				__m128 result = _mm_mul_ps(channels, m_simd_vol);
				out->left = result.m128_f32[0];
				out->right = result.m128_f32[1];
				out->center = result.m128_f32[2];*/
			}
                break;
            case AudioFrame::Type::eT_af31:
			{
				SET_BUFFERS(af31);
				SET_CHANNEL(left);
				SET_CHANNEL(right);
				SET_CHANNEL(center);
				SET_CHANNEL(sub);

				/*__m128 channels;
				channels.m128_f32[0] = out->left;
				channels.m128_f32[1] = out->right;
				channels.m128_f32[2] = out->center;
				channels.m128_f32[3] = out->sub;

				__m128 result = _mm_mul_ps(channels, m_simd_vol);
				out->left = result.m128_f32[0];
				out->right = result.m128_f32[1];
				out->center = result.m128_f32[2];
				out->sub = result.m128_f32[3];*/
			}
                break;
            case AudioFrame::Type::eT_af4:
			{
				SET_BUFFERS(af4);
				SET_CHANNEL(left);
				SET_CHANNEL(right);
				SET_CHANNEL(bleft);
				SET_CHANNEL(bright);

				/*__m128 channels;
				channels.m128_f32[0] = out->left;
				channels.m128_f32[1] = out->right;
				channels.m128_f32[2] = out->bleft;
				channels.m128_f32[3] = out->bright;

				__m128 result = _mm_mul_ps(channels, m_simd_vol);
				out->left = result.m128_f32[0];
				out->right = result.m128_f32[1];
				out->bleft = result.m128_f32[2];
				out->bright = result.m128_f32[3];*/
			}
                break;
            case AudioFrame::Type::eT_af41:
			{
				SET_BUFFERS(af41);
				SET_CHANNEL(left);
				SET_CHANNEL(right);
				SET_CHANNEL(bleft);
				SET_CHANNEL(bright);
				SET_CHANNEL(sub);

				/*__m128 channels;
				channels.m128_f32[0] = out->left;
				channels.m128_f32[1] = out->right;
				channels.m128_f32[2] = out->bleft;
				channels.m128_f32[3] = out->bright;

				__m128 result = _mm_mul_ps(channels, m_simd_vol);
				out->left = result.m128_f32[0];
				out->right = result.m128_f32[1];
				out->bleft = result.m128_f32[2];
				out->bright = result.m128_f32[3];*/
			}
                break;
			case AudioFrame::Type::eT_af5:
			{
				SET_BUFFERS(af5);
				SET_CHANNEL(left);
				SET_CHANNEL(right);
				SET_CHANNEL(center);
				SET_CHANNEL(bleft);
				SET_CHANNEL(bright);

				/*__m128 channels;
				channels.m128_f32[0] = out->left;
				channels.m128_f32[1] = out->right;
				channels.m128_f32[2] = out->center;
				channels.m128_f32[3] = out->bleft;

				__m128 result = _mm_mul_ps(channels, m_simd_vol);
				out->left = result.m128_f32[0];
				out->right = result.m128_f32[1];
				out->center = result.m128_f32[2];
				out->bleft = result.m128_f32[3];

				channels.m128_f32[0] = out->bright;
				channels.m128_f32[1] = 0.0f;
				channels.m128_f32[2] = 0.0f;
				channels.m128_f32[3] = 0.0f;

				result = _mm_mul_ps(channels, m_simd_vol);
				out->bright = result.m128_f32[0];*/
			}
                break;
            case AudioFrame::Type::eT_af51:
			{
				SET_BUFFERS(af51);
				SET_CHANNEL(left);
				SET_CHANNEL(right);
				SET_CHANNEL(center);
				SET_CHANNEL(bleft);
				SET_CHANNEL(bright);
				SET_CHANNEL(sub);

				/*__m128 channels;
				channels.m128_f32[0] = out->left;
				channels.m128_f32[1] = out->right;
				channels.m128_f32[2] = out->center;
				channels.m128_f32[3] = out->bleft;

				__m128 result = _mm_mul_ps(channels, m_simd_vol);
				out->left = result.m128_f32[0];
				out->right = result.m128_f32[1];
				out->center = result.m128_f32[2];
				out->bleft = result.m128_f32[3];

				channels.m128_f32[0] = out->bright;
				channels.m128_f32[1] = out->sub;
				channels.m128_f32[2] = 0.0f;
				channels.m128_f32[3] = 0.0f;

				result = _mm_mul_ps(channels, m_simd_vol);
				out->bright = result.m128_f32[0];
				out->sub = result.m128_f32[1];*/
			}
                break;
            case AudioFrame::Type::eT_af7:
			{
				SET_BUFFERS(af7);
				SET_CHANNEL(left);
				SET_CHANNEL(right);
				SET_CHANNEL(center);
				SET_CHANNEL(sleft);
				SET_CHANNEL(sright);
				SET_CHANNEL(bleft);
				SET_CHANNEL(bright);

				/*__m128 channels;
				channels.m128_f32[0] = out->left;
				channels.m128_f32[1] = out->right;
				channels.m128_f32[2] = out->center;
				channels.m128_f32[3] = out->bleft;

				__m128 result = _mm_mul_ps(channels, m_simd_vol);
				out->left = result.m128_f32[0];
				out->right = result.m128_f32[1];
				out->center = result.m128_f32[2];
				out->bleft = result.m128_f32[3];

				channels.m128_f32[0] = out->bright;
				channels.m128_f32[1] = out->sleft;
				channels.m128_f32[2] = out->sright;
				channels.m128_f32[3] = 0.0f;

				result = _mm_mul_ps(channels, m_simd_vol);
				out->bright = result.m128_f32[0];
				out->sleft = result.m128_f32[1];
				out->sright = result.m128_f32[2];*/
			}
                break;
            case AudioFrame::Type::eT_af71:
			{
				SET_BUFFERS(af71);
				SET_CHANNEL(left);
				SET_CHANNEL(right);
				SET_CHANNEL(center);
				SET_CHANNEL(sleft);
				SET_CHANNEL(sright);
				SET_CHANNEL(bleft);
				SET_CHANNEL(bright);
				SET_CHANNEL(sub);

				/*__m128 channels;
				channels.m128_f32[0] = out->left;
				channels.m128_f32[1] = out->right;
				channels.m128_f32[2] = out->center;
				channels.m128_f32[3] = out->bleft;

				__m128 result = _mm_mul_ps(channels, m_simd_vol);
				out->left = result.m128_f32[0];
				out->right = result.m128_f32[1];
				out->center = result.m128_f32[2];
				out->bleft = result.m128_f32[3];

				channels.m128_f32[0] = out->bright;
				channels.m128_f32[1] = out->sleft;
				channels.m128_f32[2] = out->sright;
				channels.m128_f32[3] = out->sub;

				result = _mm_mul_ps(channels, m_simd_vol);
				out->bright = result.m128_f32[0];
				out->sleft = result.m128_f32[1];
				out->sright = result.m128_f32[2];
				out->sub = result.m128_f32[3];*/
			}
                break;
            }
			//this->m_mutex.unlock();
#			undef SET_BUFFERS
#			undef SET_CHANNEL
		}
    }
}