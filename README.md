# gm_8bit
A module for manipulating voice data in Garry's Mod.

# What does it do?
gm_8bit is designed to be a starting point for any kind of voice stream manipulation you might want to do on a Garry's Mod server (or any source engine server, with a bit of adjustment).

gm_8bit can decompress and recompress steam voice packets. It includes an SV_BroadcastVoiceData hook to allow server operators to incercept and manipulate this voice data. It makes several things possible, including:
* Relaying server voice data to external locations
* Performing voice recognition and producing transcripts
* Recording voice data in compressed or uncompressed form
* Applying transformation to user voice streams, for example pitch correction, noise suppression, or gain control.

gm_8bit currently has reference implementations for relaying voice data and applying transformations to voice streams. See the `voice-relay` repository for an example implementation of a server that uses gm_8bit to relay server voice communications to a discord channel.

# Builds
Both windows and linux builds are available with every commit. See the actions page.

# API
`eightbit.EnableBroadcast(bool)` Sets whether the module should relay voice packets to `localhost:4000`.

`eightbit.SetBroadcastIP(string)` Controls what IP the module should relay voice packets to, if broadcast is enabled.

`eightbit.SetBroadcastPort(number)` Controls what port the module should relay voice packets to, if broadcast is enabled.

`eightbit.EnableEffects(userid, number)` Sets whether to enable audio effects for a given userid. Takes 1 (true) or 0 (false).

`eightbit.SetSampleRate(number)` Sets the gain multiplier to apply to affected userids.

# Example
```lua
hook.Add("ApplyVoiceEffect", "test", function(userId, buffer, count)
    local ply = Player(userId)
    print(string.format("%s is speaking!" ply:Name()))
    return buffer
end)
```

