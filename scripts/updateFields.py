import requests

BASE_URL = "http://192.168.0.243:8090"
COLLECTION = "posts"
FIELD = "field"
NEW_VALUE = "u67379k9vu56d8n"

# fetch records with empty field
resp = requests.get(f"{BASE_URL}/api/collections/{COLLECTION}/records", params={
    "filter": f'{FIELD}=""'
})
resp.raise_for_status()
records = resp.json()["items"]

# update each record
for rec in records:
    rec_id = rec["id"]
    patch_url = f"{BASE_URL}/api/collections/{COLLECTION}/records/{rec_id}"
    r = requests.patch(patch_url, json={FIELD: NEW_VALUE})
    r.raise_for_status()
    print(f"Updated {rec_id}")

