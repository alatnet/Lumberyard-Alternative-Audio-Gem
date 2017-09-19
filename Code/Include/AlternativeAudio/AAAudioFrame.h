#pragma once

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/RTTI/TypeInfo.h>
#include <AzCore\RTTI\RTTI.h>

//#define BIT(x)  1 << x
namespace AlternativeAudio {
    namespace AudioFrame {
		//base type
		typedef void Frame;

		//mono channel
		struct af1 {
			float mono{ 0.0f };
		};

		// 2 channel (left, right) - common
		struct af2 {
			float left{ 0.0f };
			float right{ 0.0f };
		};

		// 2.1 channel (left, right, sub)
		struct af21 {
			float left{ 0.0f };
			float right{ 0.0f };
			float sub{ 0.0f };
		};

		// 3 channel (left, right, center)
		struct af3 {
			float left{ 0.0f };
			float center{ 0.0f };
			float right{ 0.0f };
		};

		// 3.1 channel (left, right, center, subwoofer)
		struct af31 {
			float left{ 0.0f };
			float center{ 0.0f };
			float right{ 0.0f };
			float sub{ 0.0f };
		};
		
		//4 channel (front left, front right, back left, back right)
		struct af4 {
			union {
				struct {
					float left;
					float right;
				} front;
				struct {
					float left;
					float right;
				};
			};
			union {
				struct {
					float left;
					float right;
				} back;
				struct {
					float bleft;
					float bright;
				};
			};
		};

		//4 channel (front left, front right, back left, back right, subwoofer)
		struct af41 {
			union {
				struct {
					float left;
					float right;
				} front;
				struct {
					float left;
					float right;
				};
			};
			union {
				struct {
					float left;
					float right;
				} back;
				struct {
					float bleft;
					float bright;
				};
			};
			float sub;
		};

		// 5 channel (left, right, center, back left, back right)
		struct af5 {
			/*union {
				struct {
					float left;
					float right;
				} front;
				struct {
					float left;
					float right;
				};
			};*/
			float left;
			float center;
			float right;
			union {
				struct {
					float left;
					float right;
				} back;
				struct {
					float bleft;
					float bright;
				};
			};
		};

		// 5.1 channel (left, right, center, back left, back right, subwoofer)
		struct af51 {
			/*union {
				struct {
					float left;
					float right;
				} front;
				struct {
					float left;
					float right;
				};
			};*/
			float left;
			float center;
			float right;
			union {
				struct {
					float left;
					float right;
				} back;
				struct {
					float bleft;
					float bright;
				};
			};
			float sub;
		};

		// 7 channel  (left, right, center, side left, side right, back left, back right)
		struct af7 {
			/*union {
				struct {
					float left;
					float right;
				} front;
				struct {
					float left;
					float right;
				};
			};*/
			float left;
			float center;
			float right;
			union {
				struct {
					float left;
					float right;
				} side;
				struct {
					float sleft;
					float sright;
				};
			};
			union {
				struct {
					float left;
					float right;
				} back;
				struct {
					float bleft;
					float bright;
				};
			};
		};

		// 7.1 channel (left, right, center, side left, side right, back left, back right, subwoofer)
		struct af71 {
			/*union {
				struct {
					float left;
					float right;
				} front;
				struct {
					float left;
					float right;
				};
			};*/
			float left;
			float center;
			float right;
			union {
				struct {
					float left;
					float right;
				} side;
				struct {
					float sleft;
					float sright;
				};
			};
			union {
				struct {
					float left;
					float right;
				} back;
				struct {
					float bleft;
					float bright;
				};
			};
			float sub;
		};

		//audio frame types
		enum Type {
			eT_af1,
			eT_af2,
			eT_af21,
			eT_af3,
			eT_af31,
			eT_af4,
			eT_af41,
			eT_af5,
			eT_af51,
			eT_af7,
			eT_af71
		};

		namespace Deinterlaced {
			struct af2 {
				float * left{ nullptr };
				float * right{ nullptr };
			};

			struct af21 {
				float * left{ nullptr };
				float * right{ nullptr };
				float * sub{ nullptr };
			};

			struct af3 {
				float * left{ nullptr };
				float * center{ nullptr };
				float * right{ nullptr };
			};

			struct af31 {
				float * left{ nullptr };
				float * center{ nullptr };
				float * right{ nullptr };
				float * sub{ nullptr };
			};

			struct af4 {
				union {
					struct {
						float * left;
						float * right;
					} front;
					struct {
						float * left;
						float * right;
					};
				};
				union {
					struct {
						float * left;
						float * right;
					} back;
					struct {
						float * bleft;
						float * bright;
					};
				};
			};

			struct af41 {
				union {
					struct {
						float * left;
						float * right;
					} front;
					struct {
						float * left;
						float * right;
					};
				};
				union {
					struct {
						float * left;
						float * right;
					} back;
					struct {
						float * bleft;
						float * bright;
					};
				};
				float * sub;
			};
			
			struct af5 {
				/*union {
					struct {
						float * left;
						float * right;
					} front;
					struct {
						float * left;
						float * right;
					};
				};*/
				float * left;
				float * center;
				float * right;
				union {
					struct {
						float * left;
						float * right;
					} back;
					struct {
						float * bleft;
						float * bright;
					};
				};
			};

			struct af51 {
				/*union {
					struct {
						float * left;
						float * right;
					} front;
					struct {
						float * left;
						float * right;
					};
				};*/
				float * left;
				float * center;
				float * right;
				union {
					struct {
						float * left;
						float * right;
					} back;
					struct {
						float * bleft;
						float * bright;
					};
				};
				float * sub;
			};

			struct af7 {
				/*union {
					struct {
						float * left;
						float * right;
					} front;
					struct {
						float * left;
						float * right;
					};
				};*/
				float * left;
				float * center;
				float * right;
				union {
					struct {
						float * left;
						float * right;
					} side;
					struct {
						float * sleft;
						float * sright;
					};
				};
				union {
					struct {
						float * left;
						float * right;
					} back;
					struct {
						float * bleft;
						float * bright;
					};
				};
			};

			struct af71 {
				/*union {
					struct {
						float * left;
						float * right;
					} front;
					struct {
						float * left;
						float * right;
					};
				};*/
				float * left;
				float * center;
				float * right;
				union {
					struct {
						float * left;
						float * right;
					} side;
					struct {
						float * sleft;
						float * sright;
					};
				};
				union {
					struct {
						float * left;
						float * right;
					} back;
					struct {
						float * bleft;
						float * bright;
					};
				};
				float * sub;
			};
		}
	}

	struct AudioSourceTime {
		int hrs{ 0 };
		int minutes{ 0 };
		double sec{ 0.0 };
		double totalSec{ 0.0 };
		AZ_RTTI(AudioSourceTime, "{90F357E5-05D2-4B41-B957-F12B62E28979}");

		int GetHours() { return this->hrs; }
		int GetMinutes() { return this->minutes; }
		double GetSeconds() { return this->sec; }
		double GetTotalSeconds() { return this->totalSec; }

		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<AudioSourceTime>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
			behaviorContext->Class<AudioSourceTime>("AAAudioSourceTime")
				->Method("GetHours", &AudioSourceTime::GetHours)
				->Method("GetMinutes", &AudioSourceTime::GetMinutes)
				->Method("GetSeconds", &AudioSourceTime::GetSeconds)
				->Method("GetTotalSeconds", &AudioSourceTime::GetTotalSeconds)
				->Constant("hrs", &AudioSourceTime::GetHours)
				->Constant("minutes", &AudioSourceTime::GetMinutes)
				->Constant("sec", &AudioSourceTime::GetSeconds)
				->Constant("totalSec", &AudioSourceTime::GetTotalSeconds);
		}
	};

	#define RESERVED_BIT(id,bit) eAF_Reserved##id = BIT64(##bit##)
	enum AASourceFlags {
		eAF_None = 0,
		eAF_Loop = BIT64(0),
		eAF_LoopSection = BIT64(1), //not used at the moment
		eAF_PausedOnStart = BIT64(2),
		eAF_Deinterlaced = BIT64(3),
		RESERVED_BIT(0, 4),
		RESERVED_BIT(1, 5),
		RESERVED_BIT(2, 6),
		RESERVED_BIT(3, 7),
		RESERVED_BIT(4, 8)
	};
	#undef RESERVED_BIT
}

namespace AZ {
	AZ_TYPE_INFO_SPECIALIZE(AlternativeAudio::AudioFrame::Type, "{3E6C2625-1D4B-4C84-99A8-C063ACB651EA}");
	AZ_TYPE_INFO_SPECIALIZE(AlternativeAudio::AASourceFlags, "{E298EE6D-F4D7-47E8-A413-4F959A4816A4}");
}
//#undef BIT