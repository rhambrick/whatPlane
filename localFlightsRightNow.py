import requests

def get_flights_in_bbox(lamin, lomin, lamax, lomax):
    url = (f"https://opensky-network.org/api/states/all?lamin={lamin}&lomin={lomin}&lamax={lamax}&lomax={lomax}")
    print(f"Fetching flights over area: {lamin}, {lomin}, {lamax}, {lomax}")
    print("URL: ", url)
    
    response = requests.get(url)
    if response.status_code != 200:
        print(f"Error fetching data: HTTP {response.status_code}")
        print(response.text)
        return

    data = response.json()
    states = data.get("states", [])
    if not states:
        print("No flights found in this area.")
        return
    
    print(f"Found {len(states)} flights:\n")
    for flight in states:
        callsign = (flight[1] or "").strip() or "N/A"
        origin_country = flight[2] or "N/A"

        velocity = flight[9]
        if velocity is None:
            velocity_str = "N/A"
        else:
            velocity_str = f"{velocity:.1f} m/s"

        altitude = flight[7]
        if altitude is None:
            altitude_str = "N/A"
        else:
            altitude_str = f"{altitude:.0f} m"

        print(f"✈️ {callsign:10} | {origin_country:20} | Vel: {velocity_str:>7} | Alt: {altitude_str:>7}")
    print()

if __name__ == "__main__":
    lamin = 32.6
    lomin = -97.0
    lamax = 33.2
    lomax = -96.4
    # these are for DFW general area, google the bounding box coordinates around your city as desired.
    
    get_flights_in_bbox(lamin, lomin, lamax, lomax)
