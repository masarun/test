#pragma once

#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(CtlGUID_DMFTTrace, (49B405DA, 4FDB, 43D5, A13B, A809180EAE58), \
    WPP_DEFINE_BIT(DMFT_INIT) \
    WPP_DEFINE_BIT(DMFT_CONTROL) \
    WPP_DEFINE_BIT(DMFT_GENERAL) \
    )

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) WPP_LEVEL_LOGGER(flags)  
#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags) (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)  
#define WPP_FLAG_LEVEL_LOGGER(flags,lvl) WPP_LEVEL_LOGGER(flags)  
#define WPP_FLAG_LEVEL_ENABLED(flags, lvl) (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)  

#define WPP_CHECK_LEVEL_ENABLED(flags, level) 1

// This comment block is scanned by the trace preprocessor to define our
// Trace function.
// begin_wpp config
// FUNC Trace{FLAG=DMFT_INIT}(LEVEL, MSG, ...);
// FUNC TraceEvents(LEVEL, FLAGS, MSG, ...);
// end_wpp
