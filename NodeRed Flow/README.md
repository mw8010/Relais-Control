Node-RED Flow: Digital Switch for Signal K

This flow implements a simple digital switch for a Signal K light path and can be imported into Node-RED.

How to import

1. Open Node-RED and go to the main menu → Import → "Clipboard" or "File".
2. Paste or select `digital_switch_relais_control.json` and import into the current flow.
3. Set up your Signal K server in the Signalk nodes (subscribe / send-put / send-pathvalue):
   - Replace the `source` placeholder `<YOUR_SIGNALK_WS>` with your Signal K WebSocket source or configure the Signalk node to use the server defined in Node-RED.
   - Ensure the `path` fields match the Signal K path you want to control. The flow uses the standardized placeholder `electrical.lights.<cabin>.<aft>.state` — replace `<cabin>` and `<aft>` with your own identifiers (e.g. `electrical.lights.saloon.forward.state`).
4. Deploy the flow and test by sending a boolean `true`/`false` payload to the inject node or by issuing PUTs from Signal K.

Notes

- The exported JSON was cleaned of personal websocket IDs and contains the placeholder `<YOUR_SIGNALK_WS>`; make sure to configure your Signalk connection accordingly.
- The function node filters identical states (only forwards changes).
- You may need to adapt the nodes to the installed version of `node-red-contrib-signalk`.
