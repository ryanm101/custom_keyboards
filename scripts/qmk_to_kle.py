import json
import sys
import os

def generate_kle(layout_data):
    # Sort keys by Y first (tolerance for slight misalignments), then X
    # Group into rows
    rows = []
    current_row = []
    current_y = None
    
    # Sort keys: try to guess rows by rounding Y to nearest 0.25 or 0.5
    keys = sorted(layout_data, key=lambda k: (round(k.get('y', 0) * 4), k.get('x', 0)))
    
    kle_data = []
    
    current_x = 0
    current_y = 0
    
    for key in keys:
        x = key.get('x', 0)
        y = key.get('y', 0)
        w = key.get('w', 1)
        h = key.get('h', 1)
        label = key.get('label', key.get('matrix', [0,0])[0])
        if type(label) is list:
            label = f"{label[0]},{label[1]}"
        label = str(label)
        
        # Check if we moved to a new row in coordinate space
        # A new row is typically y >= current_y + 0.5
        if y >= current_y + 0.5 and current_row:
            kle_data.append(current_row)
            current_row = []
            # In KLE, each new array implies y += 1 and x = 0
            current_y += 1
            current_x = 0
            
        props = {}
        
        # Calculate gaps
        dy = y - current_y
        dx = x - current_x
        
        if dy > 0.01:
            props['y'] = round(dy, 2)
            current_y = y
        
        if dx > 0.01:
            props['x'] = round(dx, 2)
        elif dx < -0.01:
            props['x'] = round(dx, 2)
            
        if w != 1.0:
            props['w'] = w
        if h != 1.0:
            props['h'] = h
            
        if props:
            current_row.append(props)
            
        current_row.append(label)
        
        # Update cursor
        current_x = x + w
        
    if current_row:
        kle_data.append(current_row)
        
    return kle_data

def process(file_path, out_path, layout_name=None):
    with open(file_path, 'r') as f:
        data = json.load(f)
        
    if 'layouts' in data:
        if layout_name and layout_name in data['layouts']:
            layout_key = layout_name
        else:
            layout_key = next(iter(data['layouts']))
            if 'LAYOUT_all' in data['layouts']:
                layout_key = 'LAYOUT_all'
        
        layout_data = data['layouts'][layout_key]['layout']
    else:
        print("No layouts found in", file_path)
        return
        
    kle = generate_kle(layout_data)
    
    with open(out_path, 'w') as f:
        json.dump(kle, f, indent=4)
    print(f"Generated {out_path} using layout {layout_key}")

if __name__ == "__main__":
    layout_name = sys.argv[3] if len(sys.argv) > 3 else None
    process(sys.argv[1], sys.argv[2], layout_name)
