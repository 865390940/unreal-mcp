from typing import List, Optional
from mcp.server.fastmcp import FastMCP

def register_material_tools(mcp: FastMCP):
    @mcp.tool()
    async def set_actor_material(
        actor_name: str,
        material_path: str,
        material_index: int = 0
    ) -> str:
        """
        Sets a material asset on a specific mesh component of an actor.
        
        Args:
            actor_name: Name of the actor to modify
            material_path: Path to the material asset (e.g., '/Game/Materials/M_Wood')
            material_index: Index of the material slot (default 0)
        """
        from unreal_mcp_server import send_command
        response = await send_command("set_actor_material", {
            "name": actor_name,
            "material": material_path,
            "index": material_index
        })
        return str(response)

    @mcp.tool()
    async def set_material_parameter(
        actor_name: str,
        param_name: str,
        scalar_value: Optional[float] = None,
        vector_value: Optional[List[float]] = None
    ) -> str:
        """
        Sets a scalar or vector parameter on an actor's material (creates a DMI if needed).
        
        Args:
            actor_name: Name of the actor
            param_name: Name of the parameter (e.g., 'BaseColor', 'Roughness')
            scalar_value: Scalar value (float) to set
            vector_value: List of 3 floats [R, G, B] to set as vector
        """
        from unreal_mcp_server import send_command
        params = {
            "name": actor_name,
            "param_name": param_name
        }
        if scalar_value is not None:
            params["scalar"] = scalar_value
        if vector_value is not None:
            params["vector"] = vector_value
            
        response = await send_command("set_material_parameter", params)
        return str(response)
