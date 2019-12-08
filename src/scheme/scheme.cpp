#include "maudio/maudio.h"
#include "maudio/scheme/scheme.h"

#include "nodegraph/model/graph.h"
#include "nodegraph/view/graphview.h"

#include "mutils/chibi/chibi.h"
#include "mutils/logger/logger.h"

using namespace MUtils;
using namespace MAudio;
using namespace NodeGraph;

#define TYPE_EXCEPTION(__TYPE_, __ARG_) \
    return sexp_type_exception(ctx, self, __TYPE_, __ARG_);

#define MUST_BE_EXACT(__EXACT_VAR) \
    if (!sexp_exact_integerp(__EXACT_VAR)) TYPE_EXCEPTION(SEXP_FIXNUM, __EXACT_VAR);

#define MUST_BE_FLOAT(__FLOAT_VAR) \
    if (!sexp_flonump(__FLOAT_VAR)) TYPE_EXCEPTION(SEXP_FLONUM, __FLOAT_VAR);

#define MUST_BE_PAIR(__PAIR_VAR) \
    if (!sexp_pairp(__PAIR_VAR)) TYPE_EXCEPTION(SEXP_PAIR, __PAIR_VAR)

#define MUST_BE_STRING(__STRING_VAR) \
    if (!sexp_stringp(__STRING_VAR)) TYPE_EXCEPTION(SEXP_STRING, __STRING_VAR);

#define MUST_BE_BOOL(__BOOL_VAR) \
    if (!sexp_booleanp(__BOOL_VAR)) TYPE_EXCEPTION(SEXP_BOOLEAN, __BOOL_VAR);

#define COMMON_ARGS \
    sexp ctx, sexp self, sexp_sint_t n

#define TRY(__EXPR__) \
    if (!__EXPR__) { return sexp_user_exception(ctx, self, SGE_GetError(), SEXP_NULL); }

#define sexp_to_string(arg) std::string(sexp_string_data(sexp_symbol_to_string(ctx, arg)))

Chibi scheme;
MUtils::Chibi& scheme_get_chibi()
{
    return scheme;
}

/*
(synth 'funk '((osc osc1 (freq 200) (phase .5))
               (osc osc2 (freq 260) (phase .75))
               (mix mix (split .5))
               (con (osc1 mix) (osc2 mix) (mix out)))

(synth 'funk 'play)

(func-name symbol list-singl(list-multi(symbol symbol list-pair-multi)))
*/

sexp sexp_create_node(sexp ctx, sexp self, sexp_sint_t symbol, sexp input_args)
{
    // (node '(osc s (freq 100));

    if (sexp_truep(sexp_listp(ctx, input_args)))
    {
        sexp_gc_var2(arg1, param);
        sexp_gc_preserve2(ctx, arg1, param);
       
        std::string graphName;
        std::string nodeName;
        std::string nodeType;
        
        using tParamValues = std::vector<std::variant<std::string, float, bool>>;
        std::map<std::string, tParamValues> params;

        for (; sexp_pairp(input_args); input_args = sexp_cdr(input_args))
        {
            arg1 = sexp_car(input_args);

            // First arg is the graph
            if (sexp_symbolp(arg1))
            {
                if (graphName.empty())
                {
                    graphName = sexp_to_string(arg1);
                }
                else if (nodeType.empty())
                {
                    nodeType = sexp_to_string(arg1);
                }
                else if (nodeName.empty())
                {
                    nodeName = sexp_to_string(arg1);
                }
            }
            else if (sexp_pairp(arg1))
            {
                tParamValues values;
                std::string prop;
                for (; sexp_pairp(arg1); arg1 = sexp_cdr(arg1))
                {
                    param = sexp_car(arg1);
                    if (sexp_symbolp(param))
                    {
                        if (prop.empty())
                        {
                            prop = sexp_to_string(param);
                        }
                        else
                        {
                            values.push_back(sexp_to_string(param));
                        }
                    }
                    else if (sexp_exactp(param))
                    { 
                        values.push_back((float)sexp_uint_value(param));
                    }
                    else if (sexp_flonump(param))
                    {
                        values.push_back((float)sexp_flonum_value(param));
                    }
                }
                params[prop] = values;
            }
        }

        auto itrGraph = maud.graphs.find(graphName);
        if (itrGraph == maud.graphs.end())
        {
            return sexp_user_exception(ctx, self, (std::string("Graph not found: ") + graphName).c_str(), SEXP_NULL); 
        }

        auto pNode = maud_create_node(itrGraph->second.get(), nodeType, nodeName);
        if (!pNode)
        {
            return sexp_user_exception(ctx, self, (std::string("Couldn't create node: ") + nodeType).c_str(), SEXP_NULL); 
        }

        sexp_gc_release2(ctx);
    }
    return sexp_make_fixnum(1);
}

sexp sexp_create_synth(sexp ctx, sexp self, sexp_sint_t symbol, sexp input_args)
{
    if (sexp_symbolp(input_args))
    {
        std::string strGraphName = sexp_string_data(sexp_symbol_to_string(ctx, input_args));
        maud_create_synth(strGraphName);
    }
    return sexp_make_fixnum(1);
}

void scheme_init(const std::string& basePath)
{
    chibi_init(scheme, basePath.c_str());
    sexp_define_foreign(scheme.ctx, scheme.env, "synth", 1, sexp_create_synth);
    sexp_define_foreign(scheme.ctx, scheme.env, "node", 1, sexp_create_node);
}

/*
NodeGraph::Graph graph;
void Setup()
{
    std::lock_guard<MUtilsLockableBase(std::mutex)> guard(maud.audio_mutex);

    auto pOscillator1 = graph.CreateNode<Oscillator>("Osc1", WaveTableType::Sawtooth, frequency_from_midi("c3"));
    auto pOscillator2 = graph.CreateNode<Oscillator>("Osc2", WaveTableType::Triangle, frequency_from_midi("d3"));
    auto pMix = graph.CreateNode<Mixer>("Mixer");
    auto pAudioOut = graph.CreateNode<AudioOut>("Audio_Out");
    auto pFileOut = graph.CreateNode<FileOutput>("File_Out", (file_documents_path() / "MAudio/MAudio_Out.wav"));
    auto pBitCrush = graph.CreateNode<BitCrush>("BitCrush");
    auto pChorus = graph.CreateNode<Chorus>("Chorus");
    auto pADSR = graph.CreateNode<ADSR>("ADSR");

    pOscillator1->GetFrequency()->SetLerpSamples(100000);

    // ..up to double the input frequency
    pOscillator1->GetFrequency()->SetValue(pOscillator1->GetFrequency()->GetValue<float>() * .5f);

    pOscillator1->ConnectTo(pMix, 0, 0);
    pOscillator2->ConnectTo(pMix, 0, 1);

    pMix->ConnectTo(pADSR);

    pADSR->ConnectTo(pChorus);

    pChorus->ConnectTo(pBitCrush);

    pBitCrush->ConnectTo(pAudioOut);
    pBitCrush->ConnectTo(pFileOut);
   
    LOG(INFO) << "Control Surface";
    auto pins = graph.GetControlSurface();
    std::for_each(pins.begin(), pins.end(), [](Pin* pin) { LOG(DEBUG) << pin->GetOwnerNode().GetName() + ":" + pin->GetName(); });
}
static sexp sexp_ChangeWindow(COMMON_ARGS, sexp arg0, sexp arg1, sexp arg2) {
    MUST_BE_PAIR(arg0);
    MUST_BE_EXACT(sexp_car(arg0));
    MUST_BE_EXACT(sexp_cdr(arg0));
    MUST_BE_STRING(arg1);
    MUST_BE_BOOL(arg2);
    SGE_UIVec2 size = { sexp_uint_value(sexp_car(arg0)),
                        sexp_uint_value(sexp_cdr(arg0)) };

static sexp sexp_IsRunning(COMMON_ARGS) {
    return sexp_make_boolean(SGE_IsRunning());
}

static sexp sexp_SetRefreshColor(COMMON_ARGS, sexp arg0) {
    SGE_Color color = {
        sexp_uint_value(sexp_vector_ref(arg0, SEXP_ZERO)),
        sexp_uint_value(sexp_vector_ref(arg0, SEXP_ONE)),
        sexp_uint_value(sexp_vector_ref(arg0, SEXP_TWO)),
        sexp_uint_value(sexp_vector_ref(arg0, SEXP_THREE))
    };
    SGE_SetRefreshColor(&color);
    return SEXP_NULL;
}

static sexp sexp_GetWindowSize(COMMON_ARGS) {
    SGE_UIVec2 size = SGE_GetWindowSize();
    return sexp_cons(ctx,
                     sexp_make_unsigned_integer(ctx, size.x),
                     sexp_make_unsigned_integer(ctx, size.y));
}

static sexp sexp_ChangeWindow(COMMON_ARGS, sexp arg0, sexp arg1, sexp arg2) {
    MUST_BE_PAIR(arg0);
    MUST_BE_EXACT(sexp_car(arg0));
    MUST_BE_EXACT(sexp_cdr(arg0));
    MUST_BE_STRING(arg1);
    MUST_BE_BOOL(arg2);
    SGE_UIVec2 size = { sexp_uint_value(sexp_car(arg0)),
                        sexp_uint_value(sexp_cdr(arg0)) };
    TRY(SGE_ChangeWindow(&size, sexp_string_data(arg1), sexp_unbox_boolean(arg2)));
    return SEXP_NULL;
}

static sexp sexp_EntityCreate(COMMON_ARGS) {
    SGE_GUID entity;
    SGE_EntityCreate(&entity);
    return sexp_make_unsigned_integer(ctx, entity);
}

static sexp sexp_EntityClone(COMMON_ARGS, sexp arg0) {
    MUST_BE_EXACT(arg0);
    SGE_GUID clone;
    TRY(SGE_EntityClone(sexp_uint_value(arg0), &clone));
    return sexp_make_unsigned_integer(ctx, clone);
}

static sexp sexp_EntityAddAttrib(COMMON_ARGS, sexp arg0, sexp arg1) {
    MUST_BE_EXACT(arg0);
    MUST_BE_EXACT(arg1);
    TRY(SGE_EntityAddAttribute(sexp_uint_value(arg0), sexp_uint_value(arg1)));
    return arg0;
}

static sexp sexp_EntitySetAnimation(COMMON_ARGS, sexp arg0, sexp arg1) {
    MUST_BE_EXACT(arg0);
    MUST_BE_EXACT(arg1);
    TRY(SGE_EntitySetAnimation(sexp_uint_value(arg0), sexp_uint_value(arg1)));
    return arg0;
}

static sexp sexp_EntitySetKeyframe(COMMON_ARGS, sexp arg0, sexp arg1) {
    MUST_BE_EXACT(arg0);
    MUST_BE_EXACT(arg1);
    TRY(SGE_EntitySetKeyframe(sexp_uint_value(arg0), sexp_uint_value(arg1)));
    return arg0;
}

static sexp sexp_EntitySetPosition(COMMON_ARGS, sexp arg0, sexp arg1) {
    MUST_BE_EXACT(arg0);
    MUST_BE_PAIR(arg1);
    sexp first = sexp_car(arg1);
    sexp second = sexp_cdr(arg1);
    MUST_BE_FLOAT(first);
    MUST_BE_FLOAT(second);
    SGE_Vec2 position = { sexp_flonum_value(first), sexp_flonum_value(second) };
    const SGE_GUID entity = sexp_uint_value(arg0);
    TRY(SGE_EntitySetPosition(entity, &position));
    return arg0;
}

static sexp sexp_EntitySetScale(COMMON_ARGS, sexp arg0, sexp arg1) {
    MUST_BE_EXACT(arg0);
    MUST_BE_PAIR(arg1);
    sexp first = sexp_car(arg1);
    sexp second = sexp_cdr(arg1);
    MUST_BE_FLOAT(first);
    MUST_BE_FLOAT(second);
    SGE_Vec2 scale = { sexp_flonum_value(first), sexp_flonum_value(second) };
    TRY(SGE_EntitySetScale(sexp_uint_value(arg0), &scale));
    return arg0;
}

static sexp sexp_EntitySetBlendMode(COMMON_ARGS, sexp arg0, sexp arg1) {
    MUST_BE_EXACT(arg0);
    MUST_BE_EXACT(arg1);
    TRY(SGE_EntitySetBlendMode(sexp_uint_value(arg0), sexp_sint_value(arg1)));
    return arg0;
}

static sexp sexp_EntitySetZOrder(COMMON_ARGS, sexp arg0, sexp arg1) {
    MUST_BE_EXACT(arg0);
    MUST_BE_EXACT(arg1);
    TRY(SGE_EntitySetZOrder(sexp_uint_value(arg0), sexp_uint_value(arg1)));
    return arg0;
}

static sexp sexp_EntitySetColor(COMMON_ARGS, sexp arg0, sexp arg1) {
    MUST_BE_EXACT(arg0);
    SGE_Color color = {
        sexp_uint_value(sexp_vector_ref(arg1, SEXP_ZERO)),
        sexp_uint_value(sexp_vector_ref(arg1, SEXP_ONE)),
        sexp_uint_value(sexp_vector_ref(arg1, SEXP_TWO)),
        sexp_uint_value(sexp_vector_ref(arg1, SEXP_THREE))
    };
    TRY(SGE_EntitySetColor(sexp_uint_value(arg0), &color));
    return arg0;
}

static sexp sexp_EntityGetPosition(COMMON_ARGS, sexp arg0) {
    MUST_BE_EXACT(arg0);
    SGE_Vec2 position;
    TRY(SGE_EntityGetPosition(sexp_uint_value(arg0), &position));
    return sexp_cons(ctx,
                     sexp_make_flonum(ctx, position.x),
                     sexp_make_flonum(ctx, position.y));
}

static sexp sexp_EntityGetKeyframe(COMMON_ARGS, sexp arg0) {
    MUST_BE_EXACT(arg0);
    SGE_Keyframe keyframe;
    TRY(SGE_EntityGetKeyframe(sexp_uint_value(arg0), &keyframe));
    return sexp_make_unsigned_integer(ctx, keyframe);
}

static sexp sexp_EntityRemove(COMMON_ARGS, sexp arg0) {
    MUST_BE_EXACT(arg0);
    TRY(SGE_EntityRemove(sexp_uint_value(arg0)));
    return SEXP_NULL;
}

static sexp sexp_EntityRemoveAttrib(COMMON_ARGS, sexp arg0, sexp arg1) {
    MUST_BE_EXACT(arg0);
    MUST_BE_EXACT(arg1);
    TRY(SGE_EntityRemoveAttribute(sexp_uint_value(arg0), sexp_uint_value(arg1)));
    return arg0;
}

static sexp sexp_CameraSetTarget(COMMON_ARGS, sexp arg0) {
    MUST_BE_EXACT(arg0);
    TRY(SGE_CameraSetTarget(sexp_uint_value(arg0)));
    return SEXP_NULL;
}

static sexp sexp_CameraSetCenter(COMMON_ARGS, sexp arg0) {
    MUST_BE_PAIR(arg0);
    sexp first = sexp_car(arg0);
    sexp second = sexp_cdr(arg0);
    MUST_BE_FLOAT(first);
    MUST_BE_FLOAT(second);
    SGE_Vec2 position = { sexp_flonum_value(first), sexp_flonum_value(second) };
    SGE_CameraSetCenter(&position);
    return SEXP_NULL;
}

static sexp sexp_CameraSetSpringiness(COMMON_ARGS, sexp arg0) {
    MUST_BE_FLOAT(arg0);
    SGE_CameraSetSpringiness(sexp_flonum_value(arg0));
    return SEXP_NULL;
}

static sexp sexp_CameraSetZoom(COMMON_ARGS, sexp arg0) {
    MUST_BE_FLOAT(arg0);
    SGE_CameraSetZoom(sexp_flonum_value(arg0));
    return SEXP_NULL;
}

static sexp sexp_CameraGetViewSize(COMMON_ARGS) {
    SGE_Vec2 position = SGE_CameraGetViewSize();
    return sexp_cons(ctx,
                     sexp_make_flonum(ctx, position.x),
                     sexp_make_flonum(ctx, position.y));
}

static sexp sexp_TimerCreate(COMMON_ARGS) {
    SGE_GUID timer;
    TRY(SGE_TimerCreate(&timer));
    return sexp_make_unsigned_integer(ctx, timer);
}

static sexp sexp_TimerReset(COMMON_ARGS, sexp arg0) {
    MUST_BE_EXACT(arg0);
    SGE_USec elapsed;
    TRY(SGE_TimerReset(sexp_uint_value(arg0), &elapsed));
    return sexp_make_unsigned_integer(ctx, elapsed);
}

static sexp sexp_TimerRemove(COMMON_ARGS, sexp arg0) {
    MUST_BE_EXACT(arg0);
    TRY(SGE_TimerRemove(sexp_uint_value(arg0)));
    return SEXP_NULL;
}

static sexp sexp_AnimationCreate(COMMON_ARGS, sexp arg0, sexp arg1, sexp arg2, sexp arg3) {
    MUST_BE_STRING(arg0);
    MUST_BE_PAIR(arg1);
    MUST_BE_PAIR(arg2);
    MUST_BE_PAIR(arg3);
    SGE_GUID animation;
    SGE_IVec2 start = {
        sexp_uint_value(sexp_car(arg1)),
        sexp_uint_value(sexp_cdr(arg1))
    };
    SGE_IVec2 frameSize = {
        sexp_uint_value(sexp_car(arg2)),
        sexp_uint_value(sexp_cdr(arg2))
    };
    SGE_Vec2 origin = {
        sexp_flonum_value(sexp_car(arg3)),
        sexp_flonum_value(sexp_cdr(arg3))
    };
    TRY(SGE_AnimationCreate(&animation,
                            sexp_string_data(arg0),
                            start, frameSize, origin));
    return sexp_make_unsigned_integer(ctx, animation);
}

static sexp sexp_CollisionTest(COMMON_ARGS) {
    TRY(SGE_CollisionTest());
    return SEXP_NULL;
}

static sexp sexp_PollEvent(COMMON_ARGS) {
    SGE_EventHolder holder;
    if (SGE_PollEvents(&holder)) {
        switch (holder.code) {
        case SGE_EventCode_TextEntered: {
            sexp result = sexp_make_vector(ctx, SEXP_TWO, SEXP_NULL);
            sexp_vector_set(result, SEXP_ZERO,
                            sexp_make_integer(ctx, holder.code));
            sexp_vector_set(result, SEXP_ONE,
                            sexp_make_unsigned_integer(ctx, holder.event.text.unicode));
            return result;
        }
            
        case SGE_EventCode_KeyReleased:
        case SGE_EventCode_KeyPressed: {
            sexp result = sexp_make_vector(ctx, SEXP_TWO, SEXP_NULL);
            sexp_vector_set(result, SEXP_ZERO,
                            sexp_make_integer(ctx, holder.code));
            sexp_vector_set(result, SEXP_ONE,
                            sexp_make_integer(ctx, holder.event.key.keyCode));
            return result;
        }

        case SGE_EventCode_Collision: {
            sexp result = sexp_make_vector(ctx, SEXP_THREE, SEXP_NULL);
            sexp_vector_set(result, SEXP_ZERO,
                            sexp_make_integer(ctx, holder.code));
            sexp_vector_set(result, SEXP_ONE,
                            sexp_make_unsigned_integer(ctx, holder.event.collision.first));
            sexp_vector_set(result, SEXP_ONE,
                            sexp_make_unsigned_integer(ctx, holder.event.collision.second));
            return result;
        };
        }
    }
    return SEXP_NULL;
}

static sexp sexp_Microsleep(COMMON_ARGS, sexp arg0) {
    MUST_BE_EXACT(arg0);
    SGE_Microsleep(sexp_uint_value(arg0));
    return SEXP_NULL;
}

static sexp sexp_ResourcePath(COMMON_ARGS) {
    return sexp_c_string(ctx, SGE_ResourcePath(), -1);
}

void sexp_SGE_LibraryExport(sexp ctx, sexp env) {
#define C_EXPORTP(__STR_NAME_, __ARGC_, __C_NAME_) \
    sexp_define_foreign(ctx, env, __STR_NAME_, __ARGC_, (sexp_proc1)__C_NAME_)
#define C_EXPORTI(__STR_NAME_, __VALUE_) \
    sexp_env_define(ctx, env, sexp_intern(ctx, __STR_NAME_, sizeof(__STR_NAME_) - 1), sexp_make_integer(ctx, __VALUE_))
    C_EXPORTP("sge-is-running?", 0, sexp_IsRunning);
    C_EXPORTP("sge-set-refresh-rgba", 1, sexp_SetRefreshColor);
    C_EXPORTP("sge-window-size", 0, sexp_GetWindowSize);
    C_EXPORTP("sge-change-window", 3, sexp_ChangeWindow);
    C_EXPORTP("sge-entity-create", 0, sexp_EntityCreate);
    C_EXPORTP("sge-entity-clone", 1, sexp_EntityClone);
    C_EXPORTP("sge-entity-add-attrib", 2, sexp_EntityAddAttrib);
    C_EXPORTP("sge-entity-set-animation", 2, sexp_EntitySetAnimation);
    C_EXPORTP("sge-entity-set-keyframe", 2, sexp_EntitySetKeyframe);
    C_EXPORTP("sge-entity-set-position", 2, sexp_EntitySetPosition);
    C_EXPORTP("sge-entity-set-scale", 2, sexp_EntitySetScale);
    C_EXPORTP("sge-entity-set-blend-mode", 2, sexp_EntitySetBlendMode);
    C_EXPORTP("sge-entity-set-zorder", 2, sexp_EntitySetZOrder);
    C_EXPORTP("sge-entity-set-rgba", 2, sexp_EntitySetColor);
    C_EXPORTP("sge-entity-get-position", 1, sexp_EntityGetPosition);
    C_EXPORTP("sge-entity-get-keyframe", 1, sexp_EntityGetKeyframe);
    C_EXPORTP("sge-entity-remove", 1, sexp_EntityRemove);
    C_EXPORTP("sge-entity-remove-attrib", 2, sexp_EntityRemoveAttrib);
    C_EXPORTP("sge-camera-set-target", 1, sexp_CameraSetTarget);
    C_EXPORTP("sge-camera-set-center", 1, sexp_CameraSetCenter);
    C_EXPORTP("sge-camera-set-springiness", 1, sexp_CameraSetSpringiness);
    C_EXPORTP("sge-camera-set-zoom", 1, sexp_CameraSetZoom);
    C_EXPORTP("sge-camera-get-viewsize", 0, sexp_CameraGetViewSize);
    C_EXPORTP("sge-timer-create", 0, sexp_TimerCreate);
    C_EXPORTP("sge-timer-reset", 1, sexp_TimerReset);
    C_EXPORTP("sge-timer-remove", 1, sexp_TimerRemove);
    C_EXPORTP("sge-animation-create", 4, sexp_AnimationCreate);
    C_EXPORTP("sge-collision-test", 0, sexp_CollisionTest);
    C_EXPORTP("sge-poll-event", 0, sexp_PollEvent);
    C_EXPORTP("sge-microsleep", 1, sexp_Microsleep);
    C_EXPORTP("sge-resource-path", 0, sexp_ResourcePath);
    C_EXPORTI("sge-key-a", SGE_KeyA);
    C_EXPORTI("sge-key-b", SGE_KeyB);
    C_EXPORTI("sge-key-c", SGE_KeyC);
    C_EXPORTI("sge-key-d", SGE_KeyD);
    C_EXPORTI("sge-key-e", SGE_KeyE);
    C_EXPORTI("sge-key-f", SGE_KeyF);
    C_EXPORTI("sge-key-g", SGE_KeyG);
    C_EXPORTI("sge-key-h", SGE_KeyH);
    C_EXPORTI("sge-key-i", SGE_KeyI);
    C_EXPORTI("sge-key-j", SGE_KeyJ);
    C_EXPORTI("sge-key-k", SGE_KeyK);
    C_EXPORTI("sge-key-l", SGE_KeyL);
    C_EXPORTI("sge-key-m", SGE_KeyM);
    C_EXPORTI("sge-key-n", SGE_KeyN);
    C_EXPORTI("sge-key-o", SGE_KeyO);
    C_EXPORTI("sge-key-p", SGE_KeyP);
    C_EXPORTI("sge-key-q", SGE_KeyQ);
    C_EXPORTI("sge-key-r", SGE_KeyR);
    C_EXPORTI("sge-key-s", SGE_KeyS);
    C_EXPORTI("sge-key-t", SGE_KeyT);
    C_EXPORTI("sge-key-u", SGE_KeyU);
    C_EXPORTI("sge-key-v", SGE_KeyV);
    C_EXPORTI("sge-key-w", SGE_KeyW);
    C_EXPORTI("sge-key-x", SGE_KeyX);
    C_EXPORTI("sge-key-y", SGE_KeyY);
    C_EXPORTI("sge-key-z", SGE_KeyZ);
    C_EXPORTI("sge-key-0", SGE_KeyNum0);
    C_EXPORTI("sge-key-1", SGE_KeyNum1);
    C_EXPORTI("sge-key-2", SGE_KeyNum2);
    C_EXPORTI("sge-key-3", SGE_KeyNum3);
    C_EXPORTI("sge-key-4", SGE_KeyNum4);
    C_EXPORTI("sge-key-5", SGE_KeyNum5);
    C_EXPORTI("sge-key-6", SGE_KeyNum6);
    C_EXPORTI("sge-key-7", SGE_KeyNum7);
    C_EXPORTI("sge-key-8", SGE_KeyNum8);
    C_EXPORTI("sge-key-9", SGE_KeyNum9);
    C_EXPORTI("sge-key-esc", SGE_KeyEscape);
    C_EXPORTI("sge-key-lctrl", SGE_KeyLControl);
    C_EXPORTI("sge-key-lshift", SGE_KeyLShift);
    C_EXPORTI("sge-key-lalt", SGE_KeyLAlt);
    C_EXPORTI("sge-key-lsystem", SGE_KeyLSystem);
    C_EXPORTI("sge-key-rctrl", SGE_KeyRControl);
    C_EXPORTI("sge-key-rshift", SGE_KeyRShift);
    C_EXPORTI("sge-key-ralt", SGE_KeyRAlt);
    C_EXPORTI("sge-key-rsystem", SGE_KeyRSystem);
    C_EXPORTI("sge-key-menu", SGE_KeyMenu);
    C_EXPORTI("sge-key-lbracket", SGE_KeyLBracket);
    C_EXPORTI("sge-key-rbracket", SGE_KeyRBracket);
    C_EXPORTI("sge-key-semicolon", SGE_KeySemiColon);
    C_EXPORTI("sge-key-comma", SGE_KeyComma);
    C_EXPORTI("sge-key-period", SGE_KeyPeriod);
    C_EXPORTI("sge-key-quote", SGE_KeyQuote);
    C_EXPORTI("sge-key-slash", SGE_KeySlash);
    C_EXPORTI("sge-key-backslash", SGE_KeyBackSlash);
    C_EXPORTI("sge-key-tilde", SGE_KeyTilde);
    C_EXPORTI("sge-key-equal", SGE_KeyEqual);
    C_EXPORTI("sge-key-dash", SGE_KeyDash);
    C_EXPORTI("sge-key-space", SGE_KeySpace);
    C_EXPORTI("sge-key-return", SGE_KeyReturn);
    C_EXPORTI("sge-key-backspace", SGE_KeyBackSpace);
    C_EXPORTI("sge-key-tab", SGE_KeyTab);
    C_EXPORTI("sge-key-pageup", SGE_KeyPageUp);
    C_EXPORTI("sge-key-pagedown", SGE_KeyPageDown);
    C_EXPORTI("sge-key-end", SGE_KeyEnd);
    C_EXPORTI("sge-key-home", SGE_KeyHome);
    C_EXPORTI("sge-key-insert", SGE_KeyInsert);
    C_EXPORTI("sge-key-delete", SGE_KeyDelete);
    C_EXPORTI("sge-key-plus", SGE_KeyAdd);
    C_EXPORTI("sge-key-minus", SGE_KeySubtract);
    C_EXPORTI("sge-key-multiply", SGE_KeyMultiply);
    C_EXPORTI("sge-key-divide", SGE_KeyDivide);
    C_EXPORTI("sge-key-left", SGE_KeyLeft);
    C_EXPORTI("sge-key-right", SGE_KeyRight);
    C_EXPORTI("sge-key-up", SGE_KeyUp);
    C_EXPORTI("sge-key-down", SGE_KeyDown);
    C_EXPORTI("sge-key-num0", SGE_KeyNumpad0);
    C_EXPORTI("sge-key-num1", SGE_KeyNumpad1);
    C_EXPORTI("sge-key-num2", SGE_KeyNumpad2);
    C_EXPORTI("sge-key-num3", SGE_KeyNumpad3);
    C_EXPORTI("sge-key-num4", SGE_KeyNumpad4);
    C_EXPORTI("sge-key-num5", SGE_KeyNumpad5);
    C_EXPORTI("sge-key-num6", SGE_KeyNumpad6);
    C_EXPORTI("sge-key-num7", SGE_KeyNumpad7);
    C_EXPORTI("sge-key-num8", SGE_KeyNumpad8);
    C_EXPORTI("sge-key-num9", SGE_KeyNumpad9);
    C_EXPORTI("sge-key-f1", SGE_KeyF1);
    C_EXPORTI("sge-key-f2", SGE_KeyF2);
    C_EXPORTI("sge-key-f3", SGE_KeyF3);
    C_EXPORTI("sge-key-f4", SGE_KeyF4);
    C_EXPORTI("sge-key-f5", SGE_KeyF5);
    C_EXPORTI("sge-key-f6", SGE_KeyF6);
    C_EXPORTI("sge-key-f7", SGE_KeyF7);
    C_EXPORTI("sge-key-f8", SGE_KeyF8);
    C_EXPORTI("sge-key-f9", SGE_KeyF9);
    C_EXPORTI("sge-key-f10", SGE_KeyF10);
    C_EXPORTI("sge-key-f11", SGE_KeyF11);
    C_EXPORTI("sge-key-f12", SGE_KeyF12);
    C_EXPORTI("sge-key-f13", SGE_KeyF13);
    C_EXPORTI("sge-key-f14", SGE_KeyF14);
    C_EXPORTI("sge-key-f15", SGE_KeyF15);
    C_EXPORTI("sge-key-pause", SGE_KeyPause);
    C_EXPORTI("sge-key-count", SGE_KeyCount);
    C_EXPORTI("sge-event-text-entered", SGE_EventCode_TextEntered);
    C_EXPORTI("sge-event-key-pressed", SGE_EventCode_KeyPressed);
    C_EXPORTI("sge-event-key-released", SGE_EventCode_KeyReleased);
    C_EXPORTI("sge-event-collision", SGE_EventCode_Collision);
    C_EXPORTI("sge-blend-add", SGE_BM_Add);
    C_EXPORTI("sge-blend-multiply", SGE_BM_Multiply);
    C_EXPORTI("sge-blend-alpha", SGE_BM_Alpha);
    C_EXPORTI("sge-blend-none", SGE_BM_None);
    C_EXPORTI("sge-attrib-hidden", SGE_Attr_Hidden);
    C_EXPORTI("sge-attrib-position-absolute", SGE_Attr_PositionAbsolute);
}

void schemeEntry() {
    sexp ctx, env;
    ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
    env = sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
    sexp_SGE_LibraryExport(ctx, env);
    sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 0);
    sexp_gc_var2(mainFile, result);
    sexp_gc_preserve2(ctx, mainFile, result);
    static const char* subPath = "scripts/main.scm";
    const char* resPath = SGE_ResourcePath();
    const size_t resPathLen = strlen(resPath);
    const size_t subPathLen = strlen(subPath);
    char* fullStartPath = malloc(resPathLen + subPathLen + 1);
    if (!fullStartPath) {
        goto CLEANUP;
    }
    strncpy(fullStartPath, resPath, resPathLen);
    strncpy(fullStartPath + resPathLen, subPath, subPathLen);
    fullStartPath[resPathLen + subPathLen] = '\0';
    mainFile = sexp_c_string(ctx, fullStartPath, -1);
    free(fullStartPath);
    result = sexp_load(ctx, mainFile, NULL);
    if (sexp_exceptionp(result)) {
        printf("\n[[ Exception ]]");
        printf("\n     source: ");
        sexp_write(ctx, sexp_exception_source(result), sexp_current_output_port(ctx));
        printf("\n    message: ");
        sexp_write(ctx, sexp_exception_message(result), sexp_current_output_port(ctx));
        printf("\n  irritants: ");
        sexp_write(ctx, sexp_exception_irritants(result), sexp_current_output_port(ctx));
        printf("\n  procedure: ");
        sexp_write(ctx, sexp_exception_procedure(result), sexp_current_output_port(ctx));
        printf("\n\n");
    }
 CLEANUP:
    sexp_destroy_context(ctx);
    SGE_Exit();
}

int main(int argc, char** argv) {
    if (argc == 2) {
        SGE_ConfigureResourcePath(argv[1]);
    } else if (argc == 1) {
        puts("usage: scheme-sge <path-to-package>");
    }
    SGE_Main(schemeEntry);
    return 0;
*/
