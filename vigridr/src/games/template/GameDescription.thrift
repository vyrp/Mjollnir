namespace cpp mjollnir.vigridr

/**
 *  GameDescription that is sent to the player when he connects 
 *  It should contain the initialization info (e.g. a map description)
 */
struct GameDescription {
  1: required i32 sampleDescription
}