#pragma once

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
			float right{ 0.0f };
			float center{ 0.0f };
		};

		// 3.1 channel (left, right, center, subwoofer)
		struct af31 {
			float left{ 0.0f };
			float right{ 0.0f };
			float center{ 0.0f };
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
				float * right{ nullptr };
				float * center{ nullptr };
			};

			struct af31 {
				float * left{ nullptr };
				float * right{ nullptr };
				float * center{ nullptr };
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
	};

	enum EAudioSourceFlags {
		eAF_None = 0,
		eAF_Loop = 1 << 0,
		eAF_LoopSection = 1 << 1,
		eAF_PausedOnStart = 1 << 2
	};
}