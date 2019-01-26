﻿/* artifact.c */
extern void one_high_resistance(object_type *o_ptr);
extern void one_lordly_high_resistance(object_type *o_ptr);
extern void one_ele_resistance(object_type *o_ptr);
extern void one_dragon_ele_resistance(object_type *o_ptr);
extern void one_low_esp(object_type *o_ptr);
extern void one_resistance(object_type *o_ptr);
extern void one_ability(object_type *o_ptr);
extern void one_activation(object_type *o_ptr);
extern bool create_artifact(object_type *o_ptr, bool a_scroll);
extern int activation_index(object_type *o_ptr);
extern const activation_type* find_activation_info(object_type *o_ptr);
extern void random_artifact_resistance(object_type * o_ptr, artifact_type *a_ptr);
extern bool create_named_art(ARTIFACT_IDX a_idx, POSITION y, POSITION x);

/*** Artifact indexes (see "lib/edit/a_info.txt") ***/

/* Lites */
#define ART_GALADRIEL            1
#define ART_ELENDIL              2
#define ART_JUDGE                3
#define ART_EDISON               7
#define ART_PALANTIR             15
#define ART_STONE_LORE           17
#define ART_FLY_STONE            147
#define ART_ORB_OF_FATE          245 
/* Amulets */
#define ART_CARLAMMAS            4
#define ART_INGWE                5
#define ART_DWARVES              6
#define ART_FARAMIR              18
#define ART_BOROMIR              143
#define ART_MAGATAMA             149
#define ART_INROU                166
#define ART_NIGHT                215
#define ART_SACRED_KNIGHTS       217
#define ART_HELL                 218
#define ART_CHARMED              219
#define ART_GOGO                 220

/* Rings */
#define ART_FRAKIR               8
#define ART_TULKAS               9
#define ART_NARYA               10
#define ART_NENYA               11
#define ART_VILYA               12
#define ART_POWER               13
#define ART_AHO                 14

/* Dragon Scale */
#define ART_RAZORBACK           129
#define ART_BLADETURNER         130
#define ART_SEIRYU              201

/* Hard Armour */
#define ART_SOULKEEPER          19
#define ART_ISILDUR             20
#define ART_ROHIRRIM            21
#define ART_LOHENGRIN           22
#define ART_JULIAN              23
#define ART_ARVEDUI             24
#define ART_CASPANION           25
#define ART_GILES               168
#define ART_MORLOK              203
#define ART_VETERAN             206

/* Soft Armour */
#define ART_SHIVA_JACKET        26
#define ART_HITHLOMIR           27
#define ART_THALKETTOTH         28
#define ART_HIMRING             127
#define ART_ICANUS              131
#define ART_NAMAKE_ARMOR        183
#define ART_GHB                 192
#define ART_DASAI               200
#define ART_KESHO               204
#define ART_MILIM               246

/* Shields */
#define ART_THORIN              30
#define ART_CELEGORM            31
#define ART_ANARION             32
#define ART_GIL_GALAD           138
#define ART_YENDOR              141
#define ART_YATA                151
#define ART_EARENDIL            186
#define ART_PERSEUS             197

/* Helms and Crowns */
#define ART_INDRA               33
#define ART_CHAOS               34
#define ART_BERUTHIEL           35
#define ART_THRANDUIL           36
#define ART_THENGEL             37
#define ART_HAMMERHAND          38
#define ART_DOR                 39
#define ART_HOLHENNETH          40
#define ART_TERROR              41
#define ART_AMBER               42
#define ART_NUMENOR             132
#define ART_STONEMASK           146

/* Cloaks */
#define ART_JACK                43
#define ART_COLLUIN             44
#define ART_HOLCOLLETH          45
#define ART_THINGOL             46
#define ART_THORONGIL           47
#define ART_COLANNON            48
#define ART_LUTHIEN             49
#define ART_TUOR                50
#define ART_MOOK                205
#define ART_HEAVENLY_MAIDEN     233

/* Gloves */
#define ART_CAMBELEG            52
#define ART_CAMMITHRIM          53
#define ART_PAURHACH            54
#define ART_CORWIN              55
#define ART_PAURAEGEN           56
#define ART_PAURNEN             57
#define ART_THANOS              58
#define ART_FINGOLFIN           59
#define ART_PAURNIMMEN          185

/* Boots */
#define ART_FEANOR              60
#define ART_FLORA               61
#define ART_THROR               62
#define ART_SHIVA_BOOTS         63
#define ART_GLASS               165
#define ART_GETA                210

/* Digging */
#define ART_NAIN                211

/* Swords */
#define ART_MAEDHROS            64
#define ART_CAINE               65
#define ART_NARTHANC            66
#define ART_NIMTHANC            67
#define ART_DETHANC             68
#define ART_RILIA               69
#define ART_FIONA               70
#define ART_CALRIS              71
#define ART_GRAYSWANDIR         72
#define ART_GLAMDRING           73
#define ART_NOTHUNG             74
#define ART_ORCRIST             75
#define ART_GURTHANG            76
#define ART_ZARCUTHRA           77
#define ART_TWILIGHT            78
#define ART_GONDRICAM           79
#define ART_CRISDURIAN          80
#define ART_AGLARANG            81
#define ART_RINGIL              82
#define ART_ANDURIL             83
#define ART_WEREWINDLE          84
#define ART_CHAINSWORD          85
#define ART_FORASGIL            86
#define ART_CARETH              87
#define ART_STING               88
#define ART_SOULSWORD           89
#define ART_MERLIN              90
#define ART_DOOMCALLER          91
#define ART_VORPAL_BLADE        92
#define ART_SLAYER              123
#define ART_KUSANAGI            128
#define ART_HURIN               133
#define ART_AZAGHAL             135
#define ART_NOVA                137
#define ART_CHARIOT             140
#define ART_WORPAL_BLADE        142
#define ART_MURAMASA            144
#define ART_ZANTETSU            150
#define ART_SOULCRUSH           154
#define ART_FALIS               155
#define ART_HRUNTING            156
#define ART_ANUBIS              158
#define ART_GURENKI             160
#define ART_TAILBITER           167
#define ART_MUSASI_KATANA       171
#define ART_MUSASI_WAKIZASI     172
#define ART_QUICKTHORN          174
#define ART_TINYTHORN           175
#define ART_EXCALIBUR           176
#define ART_EXCALIPUR           177
#define ART_EXCALIBUR_J         179
#define ART_ARUNRUTH            184
#define ART_HAKABUSA            189
#define ART_STORMBRINGER        190
#define ART_NARSIL              191
#define ART_KANNU               193
#define ART_GRIMTOOTH           196
#define ART_KAMUI               198
#define ART_GOURYU              207
#define ART_EOWYN               216
#define ART_NANACHO             248
#define ART_ROBINTON            251

/* Polearms */
#define ART_THEODEN             93
#define ART_PAIN                94
#define ART_OSONDIR             95
#define ART_TIL                 96
#define ART_RUNESPEAR           97
#define ART_DESTINY             98
#define ART_HAGEN               99
#define ART_EORLINGAS           100
#define ART_DURIN               101
#define ART_EONWE               102
#define ART_BALLI               103
#define ART_LOTHARANG           104
#define ART_DWARVES_AXE         105
#define ART_BARUKKHELED         106
#define ART_WRATH               107
#define ART_ULMO                108
#define ART_AVAVIR              109
#define ART_BENKEI              152
#define ART_TAIKOBO             159
#define ART_TONBO               161
#define ART_GAEBOLG             163
#define ART_ARRYU               164
#define ART_AEGLOS              187
#define ART_BLOOD               199
#define ART_NUMAHOKO            202

/* The sword of the Dawn */
#define ART_DAWN                110

/* Hafted */
#define ART_GROND               111
#define ART_TOTILA              112
#define ART_THUNDERFIST         113
#define ART_BLOODSPIKE          114
#define ART_FIRESTAR            115
#define ART_TARATOL             116
#define ART_AULE                117
#define ART_NAR                 118
#define ART_ERIRIL              119
#define ART_GANDALF             120
#define ART_DEATHWREAKER        121
#define ART_TURMIL              122
#define ART_MJOLLNIR            136
#define ART_WINBLOWS            139
#define ART_XIAOLONG            145
#define ART_NYOIBOU             157
#define ART_JONES               162
#define ART_HYOUSIGI            169
#define ART_MATOI               170
#define ART_IRON_BALL           173
#define ART_SAMSON              178
#define ART_NAMAKE_HAMMER       181
#define ART_BOLISHOI            188
#define ART_SHUTEN_DOJI         194
#define ART_G_HAMMER            195
#define ART_AEGISFANG           208
#define ART_HERMIT              209
#define ART_GOTHMOG             212
#define ART_JIZO                213
#define ART_FUNDIN              214
#define ART_AESCULAPIUS         225

/* Bows */
#define ART_BELTHRONDING        124
#define ART_BARD                125
#define ART_BRAND               126
#define ART_CRIMSON             16
#define ART_BUCKLAND            134
#define ART_YOICHI              148
#define ART_HARAD               180
#define ART_NAMAKE_BOW          182
#define ART_ROBIN_HOOD          221
#define ART_HELLFIRE            222

/* Arrows */
#define ART_BARD_ARROW          153