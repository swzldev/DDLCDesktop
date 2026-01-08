#pragma once

/*
* MONIKA:
* 
* Emotions map:
* a		-smile
* b		-open_smile
* c		-neutral
* d		-neutral_open
* e		-soft_smile
* f		-neutral_raised
* g		-neutral_open_raised
* h		-serious
* i		-serious_open
* j		-warm_smile
* k		-joyful
* l		-nervous_laugh
* m		-nervous_smile
* n		-awkward_laugh
* o		-nervous
* p		-nervous_open
* q		-relaxed
* r		-open_relaxed
* 
* Poses map:
* left:
* 1		-arm_at_side
* 2		-casual_finger_point
* right:
* 1		-arm_at_side
* 2		-hand_on_hip
* 
* 
* YURI:
* 
* Emotions map:
* a		-smile
* a2	-head_tilt_look_forward
* b		-open_smile
* b2	-head_tilt_look_away
* c		-warm_smile
* c2	-head_tilt_face_red
* d		-joyful
* d2	-head_tilt_nervous
* e		-ooh
* e2	-head_tilt_face_red_smile
* f		-amazed
* g		-serious
* h		-serious_open_slightly
* h2	-eyes_dilated_open
* i		-serious_smile
* j		-serious_open_wide
* k		-breathe_eyes_closed
* l		-breathe_eyes_closed_2
* m		-smile_eyes_closed
* n		-nervous
* o		-nervous_look_away
* p		-distraught
* q		-nervous_laugh
* r		-angry_serious
* s		-relieved
* t		-nervous_breath
* u		-soft_smile
* v		-hesitant_worry
* w		-relieved_breath
* y		-obsessive_eyes_joy
* y2	-obsessive_nervous
* y3	-obsessive_manic_delight
* y4	-obsessive_ooh
* y5	-overjoyed
* y6	-soft_hesitation
* y7	-obsessive_angry
* 
* Poses map:
* left:
* 1		-arm_behind_back
* 2		-arm_fidget_at_chest
* right:
* 1		-arm_behind_back
* 2		-arm_fidget_at_chest
* 
* 
* NATSUKI:
* 
* Emotions map:
* a		-smile
* b		-talk
* c		-mouth_open
* d		-smile_open
* e		-talk_serious
* f		-frustrated
* g		-annoyed
* h		-flustered_open
* i		-flustered
* j		-smile_sincere
* k		-talk_sincere
* l		-joyful
* m		-concerned_open
* n		-concerned
* o		-angry
* p		-shout
* q		-look_away_open
* r		-look_away_angry
* s		-look_away
* scream-scream
* t		-cool_smile_open
* u		-concerned_look_away
* v		-cute_moan
* w		-talk_eyes_closed
* x		-angry_eyes_closed
* y		-smartass
* z		-overjoyed
* 
* Poses map:
* left:
* 1		-arm_at_side
* 2		-hand_on_hip
* 
* right:
* 1		-arm_at_side
* 2		-hand_on_hip
* 
* 
* SAYORI:
* Emotions map:
* a		-smile
* b		-neutral
* c		-talk
* d		-smile_sincere
* e		-hesitant
* f		-concern
* g		-concern_open
* h		-concern_talk
* i		-serious
* j		-serious_talk
* k		-soft_sorrow
* l		-nervous_laugh
* m		-surprised
* n		-amazed_ooh
* o		-serious_nervous
* p		-cute_moan
* q		-warm_smile
* r		-joyful
* s		-joyful_blush
* t		-sincere_cry
* u		-sad_cry
* v		-cry_blush
* w		-cry
* x		-happy
* y		-smile_sincere_blush
* 
* Poses map:
* left:
* 1		-arm_at_side
* 2		-arm_in_air
* 
* right:
* 1		-arm_at_side
* 2		-arm_in_air
* 
*/

enum class ddlc_character {
	MONIKA,
	YURI,
	NATSUKI,
	SAYORI,
};